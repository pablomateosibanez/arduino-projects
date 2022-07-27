#include <NewTone.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "TFLunaI2C.h"
#include <TFMPlus.h>  // Include TFMini Plus Library v1.4.0

#define BUZZER 12
#define LED 13

int MAX_DISTANCE_TO_DETECTED = 100;
//int MAX_DISTANCE_TO_DETECTED = 600;
#define MAX_DISTANCE_SENSOR 800
bool trigger_1 = false;
bool trigger_2 = false;
String inputString = "";
bool gate_size_recived = false;
int waitTimeToDetected = 1000;

// I2C SENSOR_1
uint16_t distance1 = 0;       
uint16_t tfFlux1 = 0;       
uint16_t tfTemp1= 0;       
TFLunaI2C tf_luna;

// UART SENSOR_2
int16_t distance2 = 0;
int16_t tfFlux2 = 0;
int16_t tfTemp2 = 0;
TFMPlus tfmP;
SoftwareSerial mySerial(5, 4);

void in_sound() {
    NewTone(BUZZER, 3046.50);
    delay(50);
    noNewTone(BUZZER);
}

void out_sound() {
    NewTone(BUZZER, 130.81);
    delay(50);
    noNewTone(BUZZER);
}

// SENSOR_2
void setupUARTSensor() {
  mySerial.begin(115200);
  delay(20);
  tfmP.begin(&mySerial);   // Initialize device library object and...
                           // pass device serial port to the object.
  
  // Send some example commands to the TFMini-Plus
  // - - Perform a system reset - - - - - - - - - - -
  
  //Serial.println( "System reset: ");
  if( tfmP.sendCommand( SYSTEM_RESET, 0))
  {
  //    Serial.println( "passed.");
  }
  //else tfmP.printReply();
  
  delay(300);  // added to allow the System Rest enough time to complete
  
  // - - Display the firmware version - - - - - - - - -
  
  //Serial.println( "Firmware version: ");
  if( tfmP.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
  {
//      Serial.println(tfmP.version[ 0]); // print three single numbers
//      Serial.println(tfmP.version[ 1]); // each separated by a dot
//      Serial.println(tfmP.version[ 2]);
  }
  //else tfmP.printReply();
  // - - Set the data frame-rate to 20Hz - - - - - - - -
  
  //Serial.println( "Data-Frame rate: ");
  if( tfmP.sendCommand( SET_FRAME_RATE, FRAME_20))
  {
  //    Serial.println(FRAME_20);
  }
  //else tfmP.printReply();
  // - - - - - - - - - - - - - - - - - - - - - - - -
  
  delay(500);            // And wait for half a second.  
}

// SENSOR_1
void setupI2CSensor() {
  // Send some example commands to the TFLuna
    // - - - - - - - - - - - - - - - - - - - - - - - -
    
    //Serial.println( "System reset: ");
    if( tf_luna.sendCommand( SYSTEM_RESET, 0))
    {
    //    Serial.println( "passed.");
    }
    //else tf_luna.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -
    
    //Serial.println( "Firmware version: ");
    if( tf_luna.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
    {
//        Serial.println(tf_luna.version[ 0]); // print three single numbers
//        Serial.println(tf_luna.version[ 1]); // each separated by a dot
//        Serial.println(tf_luna.version[ 2]);
    }
    //else tf_luna.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -
    
    //Serial.println( "Data-Frame rate: ");
    if( tf_luna.sendCommand( SET_FRAME_RATE, FRAME_250))
    {
    //    Serial.println(FRAME_250);
    }
    //else tf_luna.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -

    delay(500);
}

void getGateSize() {
  int gate_size = 0;  
  if (gate_size_recived == false) {
    if (Serial.available()) {
      inputString = Serial.readString();              
      if (inputString.length() != 0) {                  
        String G = inputString.substring(0, 5);        
        if (G == "GATE:") {
          String N = inputString.substring(5, inputString.length());                    
          gate_size = N.toInt();
          if (gate_size > 0) {                        
            gate_size_recived = true;   
            MAX_DISTANCE_TO_DETECTED = gate_size;        
            digitalWrite(LED, HIGH);
            delay(1000);     
            digitalWrite(LED, LOW);       
            Serial.print("FGATE:");
          }        
        }
      }
    }  
  }  
}

void readSensor1() {
  if (tf_luna.getData(distance1, tfFlux1, tfTemp1)) {   
    if (distance1 > 0 && distance1 <= MAX_DISTANCE_SENSOR) {
      if (!trigger_1 && distance1 <= MAX_DISTANCE_TO_DETECTED) {                  
        Serial.print("I");                               
        digitalWrite(LED, HIGH);
        in_sound();
        //trigger_1 = true;
        delay(waitTimeToDetected);            
      }                
    }          
    
    if ((distance1 >= MAX_DISTANCE_TO_DETECTED && distance1 <= MAX_DISTANCE_SENSOR) || distance1 == 0) {
      trigger_1 = false;   
    }        
  }
}

void readSensor2() {
  if (tfmP.getData(distance2, tfFlux2, tfTemp2)) {      
    if (distance2 > 0 && distance2 <= MAX_DISTANCE_SENSOR) {
      if (!trigger_2 && distance2 <= MAX_DISTANCE_TO_DETECTED) {          
        Serial.print("D");                           
        digitalWrite(LED, HIGH);
        out_sound();
        //trigger_2 = true;
        delay(waitTimeToDetected);            
      }                
    }

    if ((distance2 >= MAX_DISTANCE_TO_DETECTED && distance2 <= MAX_DISTANCE_SENSOR) || distance2 == 0) {
      trigger_2 = false;   
    }
  }
}
 
void setup() {
  Wire.begin();            
////    Wire.setClock( 400000);  // Set I2C bus speed to 'Fast' if your Arduino supports 400KHz.
  Serial.begin(9600);      
  delay(20); 
  
  setupI2CSensor();  
  setupUARTSensor();
  
  pinMode(LED, OUTPUT);    
}
 
void loop() {    
  getGateSize();  

  digitalWrite(LED, LOW);        
  noNewTone(BUZZER);  

  if (gate_size_recived) {
    readSensor1();
    readSensor2();
  } 


//  Serial.print("Distance1: ");
//  Serial.print(distance1);
//  Serial.print(" Distance2: ");
//  Serial.println(distance2);
  
//  
  //delay(1);    //  Run loop at approximately 20Hz.
}
