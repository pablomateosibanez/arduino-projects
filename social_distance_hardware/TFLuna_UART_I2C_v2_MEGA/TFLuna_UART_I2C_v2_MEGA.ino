#include <NewTone.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "TFLunaI2C.h"
#include <DFRobot_TFmini.h>

#define BUZZER 12
#define LED 13

int MAX_DISTANCE_TO_DETECTED = 100;
//int MAX_DISTANCE_TO_DETECTED = 600;
#define MAX_DISTANCE_SENSOR 800
String inputString = "";
bool gate_size_recived = false;
int waitTimeToDetected = 10;

// UART SENSOR_1
DFRobot_TFmini  TFmini;
uint16_t distance1 = 0;
SoftwareSerial mySerial(10, 11);

// I2C SENSOR_2
uint16_t distance2 = 0;       
uint16_t tfFlux2 = 0;       
uint16_t tfTemp2= 0;       
TFLunaI2C tf_luna;

struct {       
    long time;
    long releaseTime;    
    uint16_t distance;    
} info_detected_1;

struct {       
    long time;
    long releaseTime;        
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

// SENSOR_1
void setupUARTSensor() {
  TFmini.begin(mySerial);

  delay(500);
}

// SENSOR_2
void setupI2CSensor() {  
    //Serial.println( "System reset: ");
    if( tf_luna.sendCommand( SYSTEM_RESET, 0))
    {
    //    Serial.println( "passed.");
    }
    //else tf_luna.printErrorStatus();    
    
    //Serial.println( "Firmware version: ");
    if( tf_luna.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
    {
//        Serial.println(tf_luna.version[ 0]); // print three single numbers
//        Serial.println(tf_luna.version[ 1]); // each separated by a dot
//        Serial.println(tf_luna.version[ 2]);
    }
    //else tf_luna.printErrorStatus();
    
    
    //Serial.println( "Data-Frame rate: ");
    if( tf_luna.sendCommand( SET_FRAME_RATE, FRAME_250))
    {
    //    Serial.println(FRAME_250);
    }
    //else tf_luna.printErrorStatus();
    

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
  if (TFmini.measure() && tf_luna.getData(distance2, tfFlux2, tfTemp2)) {                
    distance1 = TFmini.getDistance();         

    if (distance1 > 0 && distance1 <= MAX_DISTANCE_SENSOR) {
      if (distance1 <= MAX_DISTANCE_TO_DETECTED) {          
        info_detected_1.time = millis(); 
        info_detected_1.distance = distance1;
      }                
    }

    if (distance2 > 0 && distance2 <= MAX_DISTANCE_SENSOR) {
      if (distance2 <= MAX_DISTANCE_TO_DETECTED) {                          
        info_detected_2.time = millis(); 
        info_detected_2.distance = distance2;
      }                
    }

//    Serial.print(distance1);                        
//    Serial.print(" - ");
//    Serial.println(distance2);                          
  }    

  if ((distance1 >= MAX_DISTANCE_TO_DETECTED && distance1 <= MAX_DISTANCE_SENSOR)) {    
    if (info_detected_1.time > 0) {
      //Serial.println(info_detected_1.time);      
      info_detected_1.releaseTime = millis(); 
    }
  }

  if ((distance2 >= MAX_DISTANCE_TO_DETECTED && distance2 <= MAX_DISTANCE_SENSOR)) {    
    if (info_detected_2.time > 0) {    
      //Serial.println(info_detected_2.time);                
      info_detected_2.releaseTime = millis(); 
    }
  }

  if (((distance1 >= MAX_DISTANCE_TO_DETECTED && distance1 <= MAX_DISTANCE_SENSOR) || distance1 == 0) &&
      ((distance2 >= MAX_DISTANCE_TO_DETECTED && distance2 <= MAX_DISTANCE_SENSOR) || distance2 == 0)) {

    long timeDiff = info_detected_2.time - info_detected_1.time;  
       
    if (info_detected_1.distance > 0 && timeDiff > 50) {      
      Serial.print("I");                           
      digitalWrite(LED, HIGH);
      in_sound();    
      //delay(waitTimeToDetected);            
    } 
    
    if (info_detected_2.distance > 0 && timeDiff < -50) {
//      Serial.print(info_detected_2.releaseTime);
//      Serial.print(" - ");
//      Serial.println(info_detected_1.releaseTime);
      
      Serial.print("D");                           
      digitalWrite(LED, HIGH);
      out_sound();      
      //delay(waitTimeToDetected);            
      
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
  Serial.begin(115200);      
  delay(20); 
  
  setupI2CSensor();  
  setupUARTSensor();
  
  pinMode(LED, OUTPUT);    
}
 
void loop() {    
//  getGateSize();  
//
  digitalWrite(LED, LOW);        
  noNewTone(BUZZER); 

  //if (gate_size_recived) {
    processDetection();       
  //} 

  //delay(100);
}
