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
int waitTimeToDetected = 1;
int distance1_detected = 0;
int distance2_detected = 0;
int edge_detection = 10;

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

struct {       
    long time;
    uint16_t distance;    
} info_detected_1;

struct {       
    long time;
    int16_t distance;    
} info_detected_2;

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
  if( tfmP.sendCommand( SET_FRAME_RATE, FRAME_250))
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

void processDetection() {
  if (tf_luna.getData(distance1, tfFlux2, tfTemp2, 0x10)) {
    if (distance1 > 0 && distance1 <= MAX_DISTANCE_TO_DETECTED) {          
      if (sensor_1_detected == false) {
        order[count] = 1;      
        sensor_1_detected = true;        
        count = count + 1;
      }
    }                
  }
  

  if (tf_luna.getData(distance2, tfFlux2, tfTemp2, 0x22)) {
    if (distance2 > 0 && distance2 <= MAX_DISTANCE_TO_DETECTED ) {
      if (sensor_2_detected == false) {
        order[count] = 2;      
        sensor_2_detected = true;   
        count = count + 1;     
      }
    }
  }  

  Serial.println(distance1);
  Serial.println(distance2);  

  if (((distance1 >= MAX_DISTANCE_TO_DETECTED && distance1 <= MAX_DISTANCE_SENSOR) || distance1 == 0) &&
      ((distance2 >= MAX_DISTANCE_TO_DETECTED && distance2 <= MAX_DISTANCE_SENSOR) || distance2 == 0)) {

    long timeDiff = info_detected_2.time - info_detected_1.time;    
       
    if (info_detected_1.distance > 0 && timeDiff > 0) {      
      Serial.print("I");                           
      digitalWrite(LED, HIGH);
      in_sound();    
      delay(waitTimeToDetected);            
    } 
    
    if (info_detected_2.distance > 0 && timeDiff < 0) {
      Serial.print("D");                           
      digitalWrite(LED, HIGH);
      out_sound();      
      delay(waitTimeToDetected);            
    }   
        
    info_detected_1.time = 0;
    info_detected_1.distance = 0;

    info_detected_2.time = 0;
    info_detected_2.distance = 0;        
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

  //if (gate_size_recived) {
    processDetection();       
  //} 

  delay(10);
}
