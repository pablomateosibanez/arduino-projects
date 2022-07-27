#include <EEPROM.h>


uint addr = 0;

int a = 0;
int value;

struct { 
    char ssid[50] = "";
    char pass[50] = "";    
    char uuid[50] = "";        
} wifiCredentials;

void saveWifiCredentials() {
  EEPROM.begin(512);

  const String& ssid = "ssid_1";
  const String& pass = "pass_1";  
  const String& uuid = "529afa20-15ed-47fe-afb0-caca";  
  
  strncpy(wifiCredentials.ssid, ssid.c_str(), 50);  
  strncpy(wifiCredentials.pass, pass.c_str(), 50);    
  strncpy(wifiCredentials.uuid, uuid.c_str(), 50);    
  
  EEPROM.put(addr, wifiCredentials);  
  EEPROM.commit();  
  EEPROM.end();     
}

void loadWifiCredentials() {
//  strncpy(wifiCredentials.ssid, "", 50);
//  strncpy(wifiCredentials.pass, "", 50);  
//  strncpy(wifiCredentials.uuid, "", 50);  
  EEPROM.begin(512);
  EEPROM.get(addr, wifiCredentials);
  
  Serial.println(String(wifiCredentials.ssid));
  Serial.println(String(wifiCredentials.pass));  
  Serial.println(String(wifiCredentials.uuid));    
}


void setup()
{
  Serial.begin(9600);

  delay(10000);

  //saveWifiCredentials();

  loadWifiCredentials();

//  uint addr = 0;
//
//  // fake data
//  struct { 
//    char ssid[50] = "";
//    char pass[50] = "";    
//    char uuid[50] = "";        
//  } data;
//
//  // commit 512 bytes of ESP8266 flash (for "EEPROM" emulation)
//  // this step actually loads the content (512 bytes) of flash into 
//  // a 512-byte-array cache in RAM
//  EEPROM.begin(512);
//
////  // read bytes (i.e. sizeof(data) from "EEPROM"),
////  // in reality, reads from byte-array cache
////  // cast bytes into structure called data
////  EEPROM.get(addr,data);
////  Serial.println("Old values are: "+String(data.val)+","+String(data.str));
////
////  // fiddle with the data "read" from EEPROM
////  data.val += 5;
////  if ( strcmp(data.str,"hello") == 0 )
////      strncpy(data.str, "jerry",20);
////  else 
////      strncpy(data.str, "hello",20);
//
//  // replace values in byte-array cache with modified data
//  // no changes made to flash, all in local byte-array cache
//  const String& ssid = "ssid_1";
//  const String& pass = "pass_1";  
//  const String& uuid = "529afa20-15ed-47fe-afb0-caca";  
//  
//  strncpy(data.ssid, ssid.c_str(), 50);  
//  strncpy(data.pass, pass.c_str(), 50);    
//  strncpy(data.uuid, uuid.c_str(), 50);    
//  EEPROM.put(addr, data);
//
//  // actually write the content of byte-array cache to
//  // hardware flash.  flash write occurs if and only if one or more byte
//  // in byte-array cache has been changed, but if so, ALL 512 bytes are 
//  // written to flash
//  EEPROM.commit();  
//
//  // clear 'data' structure  
//  strncpy(data.ssid, "", 50);
//  strncpy(data.pass, "", 50);  
//  strncpy(data.uuid, "", 50);  
//
//  // reload data for EEPROM, see the change
//  //   OOPS, not actually reading flash, but reading byte-array cache (in RAM), 
//  //   power cycle ESP8266 to really see the flash/"EEPROM" updated
//  EEPROM.get(addr,data);
//  Serial.println(String(data.ssid));
//  Serial.println(String(data.pass));  
//  Serial.println(String(data.uuid));  
//  //Serial.println("New values are: "+String(data.val)+","+String(data.str));
}

void loop()
{
  value = EEPROM.read(a);

  Serial.print(a);
  Serial.print("\t");
  Serial.print(value);
  Serial.println();

  a = a + 1;

  if (a == 1024)
    a = 0;

  delay(100);
}
