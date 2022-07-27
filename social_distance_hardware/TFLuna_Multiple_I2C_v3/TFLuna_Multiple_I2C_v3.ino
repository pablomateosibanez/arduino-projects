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

// UART SENSOR_1
DFRobot_TFmini  TFmini;
uint16_t distance1 = 0;
//SoftwareSerial mySerial(5, 4);
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

bool sensor_1_detected = false;
bool sensor_2_detected = false;
int order[2];
int count = 0;

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

void printDistances(uint16_t distance1, uint16_t distance2) {
  Serial.print("distance 1: ");
  Serial.print(distance1);
  Serial.print(" distance 2: ");
  Serial.println(distance2);  
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



  //printDistances(distance1, distance2);       


  if ((distance1 >= MAX_DISTANCE_TO_DETECTED && distance1 <= MAX_DISTANCE_SENSOR) && 
    (distance2 >= MAX_DISTANCE_TO_DETECTED && distance2 <= MAX_DISTANCE_SENSOR)) {   

      if (sensor_2_detected && sensor_1_detected) {
        for (int i = 0; i <= 1; i++) {      
            Serial.println(order[i]);
        }
      }

      if (order[0] == 1 && order[1] == 2) {
        //Serial.print("D");                           
        digitalWrite(LED, HIGH);
        out_sound(); 
      }

      if ((order[0] == 2) && (order[1] == 1)) {
        //Serial.print("I");                           
        digitalWrite(LED, HIGH);
        in_sound(); 
      }
                       

      sensor_1_detected = false;
      sensor_2_detected = false;
      count = 0;
      order[0] = 0;
      order[1] = 0;
  }
}

void setup() {
  Wire.begin();            
////    Wire.setClock( 400000);  // Set I2C bus speed to 'Fast' if your Arduino supports 400KHz.
  //Serial.begin(115200);      
  Serial.begin(9600);        
  delay(20); 
    
  pinMode(LED, OUTPUT);    
}
 
void loop() {    
  getGateSize();  
    
  digitalWrite(LED, LOW);    
  noNewTone(BUZZER); 

 // if (gate_size_recived) {
    processDetection();       
 // } 

 // delay(1000);
}
