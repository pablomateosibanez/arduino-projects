#include <EEPROM.h>

struct {       
    char ssid[50] = "";
    char pass[50] = "";    
    char uuid[50] = "";        
    char gateDimension[50] = "";
    char server_host[50] = "";
} wifiCredentials;

void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
  delay(5000);
  Serial.println("clearing EEPROM...");
  // write a 0 to first 1000 bytes of the EEPROM
  for (int i = 0; i < 1000; ++i) {
    EEPROM.write(i, -1);
 }
  EEPROM.commit();
  delay(5000);
  Serial.println("Reading EEPROM...");  

  int value; 
  EEPROM.begin(512);    
  value = EEPROM.read(0);
  if (value == 255 || value == 0) {
    Serial.println("EEPROM clear");
  }    
}

void loop() {
  
}
