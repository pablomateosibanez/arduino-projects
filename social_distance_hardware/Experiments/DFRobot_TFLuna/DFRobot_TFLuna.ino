 /*
  * @File  : DFRobot_TFmini_test.ino
  * @Brief : This example use TFmini to measure distance
  *         With initialization completed, we can get distance value and signal strength
  * @Copyright   [DFRobot](http://www.dfrobot.com), 2016
  *             GNU Lesser General Public License
  *
  * @version  V1.0
  * @date  2018-1-10
  */

#include <DFRobot_TFmini.h>
#include <Wire.h>
#include "TFLunaI2C.h"

SoftwareSerial mySerial(10, 11); // RX, TX

DFRobot_TFmini  TFmini;
uint16_t distance1;

// I2C SENSOR_2
uint16_t distance2 = 0;       
uint16_t tfFlux2 = 0;       
uint16_t tfTemp2= 0;       
TFLunaI2C tf_luna;

void setupI2CSensor() {  
    if( tf_luna.sendCommand( SYSTEM_RESET, 0))
    {
        Serial.println( "passed.");
    }
    else tf_luna.printErrorStatus();
    
    Serial.println( "Firmware version: ");
    if( tf_luna.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
    {
        Serial.println(tf_luna.version[ 0]); // print three single numbers
        Serial.println(tf_luna.version[ 1]); // each separated by a dot
        Serial.println(tf_luna.version[ 2]);
    }
    else tf_luna.printErrorStatus();
    
    Serial.println( "Data-Frame rate: ");
    if( tf_luna.sendCommand( SET_FRAME_RATE, FRAME_250))
    {
        Serial.println(FRAME_250);
    }
    else tf_luna.printErrorStatus();

    delay(500);
}

void setup(){
    Wire.begin();              
    Serial.begin(115200);
    TFmini.begin(mySerial);

    setupI2CSensor(); 
}

void processDetection() {
  if (TFmini.measure() && tf_luna.getData(distance2, tfFlux2, tfTemp2)) {                
    distance1= TFmini.getDistance();         

    Serial.print(distance1);                        
    Serial.print(" - ");
    Serial.println(distance2);                          
  }  
}

void loop(){
    processDetection();
    
}
