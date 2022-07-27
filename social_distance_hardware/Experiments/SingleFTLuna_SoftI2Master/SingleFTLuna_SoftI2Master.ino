#include <SoftI2CMaster.h>

uint8_t addr = 0x10;
//uint8_t addr = 0x59;
//uint8_t addr = 0x52;


byte error;

const int sdaPin = 4; // Arduino GPIO 4 is physical pin 7 of ATtiny841.
const int sclPin = 5; // Arduino GPIO 6 is physical pin 9 of ATtiny841.

SoftI2CMaster i2c_1 = SoftI2CMaster( sclPin, sdaPin, 0 );


void setup() {
  delay(3000);
  Serial.begin(9600,SERIAL_8N1);   
  
  
  error = i2c_1.beginTransmission(addr);
  i2c_1.endTransmission();

  if (error == 0) {
    Serial.println("caca");  
  }
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
