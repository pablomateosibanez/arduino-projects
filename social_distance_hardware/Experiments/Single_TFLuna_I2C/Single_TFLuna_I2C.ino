#include "TFLunaI2C.h"
#include <Wire.h>     // Arduino standard I2C/Two-Wire Library

// I2C SENSOR_1
uint16_t distance1 = 0;       
uint16_t tfFlux1 = 0;       
uint16_t tfTemp1= 0;       
TFLunaI2C tf_luna;

// SENSOR_1
void setupI2CSensor() {
  // Send some example commands to the TFLuna
    // - - - - - - - - - - - - - - - - - - - - - - - -
    
    Serial.println( "System reset: ");
    if( tf_luna.sendCommand( SYSTEM_RESET, 0))
    {
        Serial.println( "passed.");
    }
    else tf_luna.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -
    
    Serial.println( "Firmware version: ");
    if( tf_luna.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
    {
        Serial.println(tf_luna.version[ 0]); // print three single numbers
        Serial.println(tf_luna.version[ 1]); // each separated by a dot
        Serial.println(tf_luna.version[ 2]);
    }
    else tf_luna.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -
    
    Serial.println( "Data-Frame rate: ");
    if( tf_luna.sendCommand( SET_FRAME_RATE, FRAME_250))
    {
        Serial.println(FRAME_250);
    }
    else tf_luna.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -

    delay(500);
}

void setup() {
  Wire.begin();
  Serial.begin(9600);      
  delay(3000); 
  
  setupI2CSensor();  
}

void loop() {
  tf_luna.getData(distance1, tfFlux1, tfTemp1);

  Serial.println(distance1);

}
