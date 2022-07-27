#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> 

String server_host;
int red_light_pin= 4;
int green_light_pin = 2;
int blue_light_pin = 12;
const int touchPin = 16;
int contconexion = 0;
uint addr = 0;
int value; 
bool statusWifiConection = false;
bool send_gate_size = false;

struct {       
    char ssid[50] = "";
    char pass[50] = "";    
    char uuid[50] = "";        
    char gateDimension[50] = "";
    char server_host[50] = "";
} wifiCredentials;

String mensaje = "";
const char *ssidConf = "social_distance";
const char *passConf = "12345678";

WiFiClient espClient;
ESP8266WebServer server(80);
HTTPClient http;

void responseSetup(bool status) {  
  String json;
  StaticJsonDocument<1024> docjson;
  JsonObject jsonstatus = docjson.createNestedObject();
  if (status) {
    jsonstatus["status"] = "ok";
  }
  else
  {
    jsonstatus["status"] = "error";
  }
  serializeJson(docjson, json); 
  server.send(200, "text/json", json);  
  delay(10000);
}

void responseDevice() {  
  String json = "{}";  
  server.send(200, "text/json", json);    
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}

void modoconf() {
  Serial.println("Enter in setup mode...");
  RGB_color(255, 255, 0);  
  
  WiFi.softAP(ssidConf, passConf);
  IPAddress myIP = WiFi.softAPIP(); 
  Serial.print("IP del acces point: ");
  Serial.println(myIP);
  Serial.println("WebServer iniciado...");

  //server.on("/", responseSetup(true)); //esta es la pagina de configuracion

  server.on("/ping", responseDevice); // responde el dispositivo cuando se está configurando

  server.on("/setupwifi", saveWifiCredentials); //Graba en la eeprom la configuracion  

  server.on("/setupwifi/available", getAvailableNetworks); //Escanean las redes wifi disponibles
  
  server.begin();

  while (statusWifiConection == false) {        
    server.handleClient();
  }
}

void clearEEPROM() {
  EEPROM.begin(512);
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }  
}

void saveWifiCredentials() {
  EEPROM.begin(512);
  
  strncpy(wifiCredentials.ssid, server.arg("ssid").c_str(), 50);  
  strncpy(wifiCredentials.pass, server.arg("pass").c_str(), 50);    
  strncpy(wifiCredentials.uuid, server.arg("uuid").c_str(), 50);    
  strncpy(wifiCredentials.gateDimension, server.arg("gate_dimension").c_str(), 50); 
  strncpy(wifiCredentials.server_host, server.arg("server_host").c_str(), 50);      
  
  EEPROM.put(addr, wifiCredentials);  
  EEPROM.commit(); 
  EEPROM.end();    

  Serial.println(server.arg("ssid"));  
  Serial.println(server.arg("pass"));
  Serial.println(server.arg("uuid")); 
  Serial.println(server.arg("gate_dimension"));   
  Serial.println(server.arg("server_host"));     

  mensaje = "Configuracion Guardada...";  
  responseSetup(true);      
  
  if (setup_wifi() == false) {    
    modoconf();
  } 
}

void loadWifiCredentials() {
  EEPROM.begin(512);    
  EEPROM.get(addr, wifiCredentials);    

  server_host = String(wifiCredentials.server_host);
  
  Serial.println(String(wifiCredentials.ssid));
  Serial.println(String(wifiCredentials.pass));  
  Serial.println(String(wifiCredentials.uuid));    
  Serial.println(String(wifiCredentials.gateDimension));     
  Serial.println(server_host);       
}

void setDeviceAlive(String uuid) {  
  String url = server_host + "/device/alive";       
  
   if (http.begin(espClient, url)) 
   {      
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      Serial.print("[HTTP] PUT...\n");
      String postData = "uuid=" + uuid;       
      int httpCode = http.PUT(postData); 
 
      if (httpCode > 0) {
         Serial.printf("[HTTP] PUT... code: %d\n", httpCode);
 
         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            RGB_color(0, 255, 0);
            delay(350);
            RGB_color(0, 0, 0);
            String payload = http.getString();   // Obtener respuesta
            Serial.println(payload);  
         }
      }
      else {        
         RGB_color(255, 0, 0);        
         Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
 
      http.end();
   }
   else {
      Serial.printf("[HTTP} Unable to connect\n");
   }
}

//------------------------SETUP WIFI-----------------------------
bool setup_wifi() {  
  Serial.println("Setup WIFI");   
  loadWifiCredentials();
  
// Conexión WIFI  
  WiFi.mode(WIFI_STA); //para que no inicie el SoftAP en el modo normal
  WiFi.begin(wifiCredentials.ssid, wifiCredentials.pass);  

  while (WiFi.status() != WL_CONNECTED and contconexion < 60) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(250);
    Serial.print(".");
    RGB_color(255, 255, 0);
    delay(250);
    RGB_color(0, 0, 0);
  }  

  if (contconexion < 60) {  
    setDeviceAlive(wifiCredentials.uuid);    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());      
    RGB_color(0, 255, 0);
    delay(2000);
    RGB_color(0, 0, 0); 
    statusWifiConection = true;      
    
    return true;
  }   
  else
  {    
    Serial.println("");
    Serial.println("Connection error!");
    RGB_color(255, 0, 0);        
    delay(5000);    
    statusWifiConection = false;
    modoconf();
    
    return false;
  }
}

void getAvailableNetworks() {
  String json;  
  StaticJsonDocument<1024> doc;
  
  int n = WiFi.scanNetworks(); //devuelve el número de redes encontradas
  Serial.println("escaneo terminado");
  if (n == 0) { //si no encuentra ninguna red
    Serial.println("no se encontraron redes");
    mensaje = "no se encontraron redes";
  }  
  else
  {
    Serial.print(n);
    Serial.println(" redes encontradas");
    mensaje = "";
    for (int i = 0; i < n; ++i)
    {
        JsonObject obj = doc.createNestedObject();
        obj["ssid"] = WiFi.SSID(i);
        obj["rssi"] = WiFi.RSSI(i);    
        obj["channel"] = WiFi.channel(i);            
        obj["encryptionType"] = WiFi.encryptionType(i);                    
    }    

    serializeJson(doc, json); 
    Serial.println(json);
    
    server.send(200, "text/json", json); 
  }
}

void increaseCapacity(String uuid) {  
  String url = server_host + "/capacity/increase";       
  
   if (http.begin(espClient, url)) //Iniciar conexión
   {      
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      Serial.print("[HTTP] POST...\n");
      String postData = "uuid=" + uuid;       
      int httpCode = http.POST(postData);  // Realizar petición
 
      if (httpCode > 0) {
         Serial.printf("[HTTP] POST... code: %d\n", httpCode);
 
         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            RGB_color(0, 255, 0);
            delay(350);
            RGB_color(0, 0, 0);
            String payload = http.getString();   // Obtener respuesta
            Serial.println(payload);   // Mostrar respuesta por serial
         }
      }
      else {        
         RGB_color(255, 0, 0);        
         Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
 
      http.end();
   }
   else {
      Serial.printf("[HTTP} Unable to connect\n");
   }
}

void decreaseCapacity(String uuid) {  
  String url = server_host + "/capacity/decrease";   
    
   if (http.begin(espClient, url)) //Iniciar conexión
   {
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      Serial.print("[HTTP] POST...\n");
      String postData = "uuid=" + uuid;       
      int httpCode = http.POST(postData);  // Realizar petición
 
      if (httpCode > 0) {
         Serial.printf("[HTTP] POST... code: %d\n", httpCode);
 
         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            RGB_color(0, 255, 0);
            delay(350);
            RGB_color(0, 0, 0);
            String payload = http.getString();   // Obtener respuesta
            Serial.println(payload);   // Mostrar respuesta por serial
         }
      }
      else {        
         RGB_color(255, 0, 0);        
         Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
 
      http.end();
   }
   else {
      Serial.printf("[HTTP} Unable to connect\n");
   }
}

void updateCapacity(String value, String uuid) {  
   String url = server_host + "/capacity";   
    
   if (http.begin(espClient, url)) //Iniciar conexión
   {      
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      Serial.print("[HTTP] PUT...\n");
      String postData = "value=" + value + "&uuid=" + uuid;       
      int httpCode = http.PUT(postData);  // Realizar petición
 
      if (httpCode > 0) {
         Serial.printf("[HTTP] POST... code: %d\n", httpCode);
 
         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            RGB_color(0, 255, 0);
            delay(350);
            RGB_color(0, 0, 0);
            String payload = http.getString();   // Obtener respuesta
            Serial.println(payload);   // Mostrar respuesta por serial
         }
      }
      else {        
         RGB_color(255, 0, 0);        
         Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
 
      http.end();
   }
   else {
      Serial.printf("[HTTP} Unable to connect\n");
   }
}

void sendInfo(String value, String uuid) {  
   String url = server_host + "/capacity/info";   
    
   if (http.begin(espClient, url)) //Iniciar conexión
   {      
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      Serial.print("[HTTP] POST...\n");
      String postData = "value=" + value + "&uuid=" + uuid;       
      int httpCode = http.POST(postData);  // Realizar petición
 
      if (httpCode > 0) {
         Serial.printf("[HTTP] POST... code: %d\n", httpCode);
 
         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            RGB_color(0, 255, 0);
            delay(350);
            RGB_color(0, 0, 0);
            String payload = http.getString();   // Obtener respuesta
            Serial.println(payload);   // Mostrar respuesta por serial
         }
      }
      else {        
         RGB_color(255, 0, 0);        
         Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
 
      http.end();
   }
   else {
      Serial.printf("[HTTP} Unable to connect\n");
   }
}

void processCapacity() {
  String IncomingString = "";  
  String action = "";
  String infoSubString = "";
    
  if (Serial.available()) {
    IncomingString = Serial.readString();    
      
    for (int I = 0; I < IncomingString.length(); I++) {
      action = IncomingString.substring(I, I + 1);      
      if (action == "I") {            
        increaseCapacity(wifiCredentials.uuid);              
      }
      else {
        if (action == "D") {        
          decreaseCapacity(wifiCredentials.uuid);              
        }
      }  
    }

    infoSubString = IncomingString.substring(0, 1);      
    if (infoSubString == "F") {
      sendInfo(IncomingString.substring(1, IncomingString.length()), wifiCredentials.uuid);      
    }        
  }    
}

bool initSetupMode() {
  int seconds_pushed = 0;  
  int touchState = LOW; 
  bool ledOn = false;

  if (digitalRead(touchPin) == HIGH) {
    for (int i = 0; i < 10000; i++) {
      touchState = digitalRead(touchPin);        
      
      if (touchState == HIGH) {      
        seconds_pushed++;      
      } 
  
      if (seconds_pushed >= 3000) {
        ledOn = true;
        break;
      }    
  
      delay(1);
    }  
  }

  return ledOn;
}

void sendGateSize() {
  if (send_gate_size == false) {
    Serial.print("GATE:" + String(wifiCredentials.gateDimension));    
    RGB_color(0, 0, 255);        
    delay(1000);
    RGB_color(0, 0, 0);
    delay(1000);  

    send_gate_size = true;
  }  
}

void setup() {  
  // Inicia Serial
  //Serial.begin(115200);
  Serial.begin(9600);
  //pinMode(ledPin, OUTPUT);
  pinMode(touchPin, INPUT);  
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
         
  Serial.println("");     
   
  delay(10000); 

  // Check wifi credentials 
  Serial.println("Checking wifi credentials...");
  loadWifiCredentials();    
  value = EEPROM.read(addr);
  if (value == 255 || value == 0) {
    modoconf();
  }    

  // Waiting to enter on setup mode 
  Serial.println("Waiting to enter on setup mode...");  
  if (initSetupMode()) {
    modoconf();
  }
  else
  {  
    setup_wifi();
  }
}

void loop() {     
  sendGateSize();
    
  processCapacity(); 

  //delay(1);
}
