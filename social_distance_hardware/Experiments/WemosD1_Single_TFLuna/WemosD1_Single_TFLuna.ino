#include <SoftwareSerial.h>
#include <TFMPlus.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> 

WiFiClient espClient;
ESP8266WebServer server(80);
HTTPClient http;

int buzzerPin=D5; //Buzzer control port, default D5
int MAX_DISTANCE_TO_DETECTED = 100;
//int MAX_DISTANCE_TO_DETECTED = 600;
#define MAX_DISTANCE_SENSOR 800

int16_t distance2 = 0;
int16_t tfFlux2 = 0;
int16_t tfTemp2 = 0;
TFMPlus tfmP;
SoftwareSerial mySerial(4, 5);

struct {       
    long time;
    int16_t distance;    
} info_detected_2;


// SENSOR_2
void setupUARTSensor() {
  mySerial.begin(115200);
  delay(20);
  tfmP.begin(&mySerial);   // Initialize device library object and...
                           // pass device serial port to the object.
  
  // Send some example commands to the TFMini-Plus
  // - - Perform a system reset - - - - - - - - - - -
  
  Serial.println( "System reset: ");
  if( tfmP.sendCommand( SYSTEM_RESET, 0))
  {
      Serial.println( "passed.");
  }
  else tfmP.printReply();
  
  delay(300);  // added to allow the System Rest enough time to complete
  
  // - - Display the firmware version - - - - - - - - -
  
  Serial.println( "Firmware version: ");
  if( tfmP.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
  {
      Serial.println(tfmP.version[ 0]); // print three single numbers
      Serial.println(tfmP.version[ 1]); // each separated by a dot
      Serial.println(tfmP.version[ 2]);
  }
  else tfmP.printReply();
  // - - Set the data frame-rate to 20Hz - - - - - - - -
  
  Serial.println( "Data-Frame rate: ");
  if( tfmP.sendCommand( SET_FRAME_RATE, FRAME_250))
  {
      Serial.println(FRAME_20);
  }
  else tfmP.printReply();
  // - - - - - - - - - - - - - - - - - - - - - - - -
  
  delay(500);            // And wait for half a second.  
}

bool setup_wifi() {  
  int contconexion = 0;
  Serial.println("Setup WIFI");     
  
// Conexión WIFI  
  WiFi.mode(WIFI_STA); //para que no inicie el SoftAP en el modo normal
  WiFi.begin("MiFibra-98FA", "CZg3ZZ6D");  

  while (WiFi.status() != WL_CONNECTED and contconexion < 60) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    Serial.print(".");
    delay(250);
  }  

  if (contconexion < 60) {  
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());      
    delay(2000);
   
    return true;
  }   
  else
  {    
    Serial.println("");
    Serial.println("Connection error!");    
    delay(5000);    
    
    return false;
  }
}


void setup() {
  Serial.begin(9600);      
  pinMode(buzzerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  

  setup_wifi();
  delay(3000);
  setupUARTSensor();
}

void increaseCapacity(String uuid) {  
  String url = "http://localhost:8080/capacity/increase";       
  
   if (http.begin(espClient, url)) //Iniciar conexión
   {      
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      Serial.print("[HTTP] POST...\n");
      String postData = "uuid=" + uuid;       
      int httpCode = http.POST(postData);  // Realizar petición
 
      if (httpCode > 0) {
         Serial.printf("[HTTP] POST... code: %d\n", httpCode);
 
         if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {            
            String payload = http.getString();   // Obtener respuesta
            Serial.println(payload);   // Mostrar respuesta por serial
         }
      }
      else {        
         
         Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
 
      http.end();
   }
   else {
      Serial.printf("[HTTP} Unable to connect\n");
   }
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW    
  if (tfmP.getData(distance2, tfFlux2, tfTemp2)) {      
    if (distance2 > 0 && distance2 <= MAX_DISTANCE_SENSOR) {
      if (distance2 <= MAX_DISTANCE_TO_DETECTED) {          
        info_detected_2.time = millis(); 
        info_detected_2.distance = distance2;
        //tone(buzzerPin, 700, 1000);        
        increaseCapacity("caca");
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);                       // wait for a second
//        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//        delay(1000);                       // wait for a second
      }                
    }    
  }

  Serial.println(distance2);

}
