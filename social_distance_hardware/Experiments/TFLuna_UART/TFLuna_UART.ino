#include <TimedAction.h>
#include <NewTone.h>
#include <SoftwareSerial.h>

#define BUZZER 12
#define LED 13

int MAX_DISTANCE_TO_DETECTED = 100;
//int MAX_DISTANCE_TO_DETECTED = 600;
#define MAX_DISTANCE_SENSOR 800
float distance1;
float distance2;
bool trigger_1 = false;
bool trigger_2 = false;
String inputString = "";
bool gate_size_recived = false;
int waitTimeToDetected = 1000;

// UART
int strength;
int check;
int uart[9];
//int uart2[9];
int i;
const int HEADER=0x59;

SoftwareSerial portOne(3, 2);
SoftwareSerial portTwo(5, 4);


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

void readSensor1() {
  portOne.listen();  
  if (portOne.available()) {
    if (portOne.read() == HEADER) {
      uart[0] = HEADER;
      
      if (portOne.read() == HEADER) {
        uart[1] = HEADER;

        for(i = 2; i < 9; i++) {
          uart[i] = portOne.read();
        }

        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
        if (uart[8] == (check&0xff)) {
          distance1 = uart[2] + uart[3] * 256;          

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
    }        
  }  
}

void readSensor2() {
  portTwo.listen();  
  if (portTwo.available()) {
    if (portTwo.read() == HEADER) {
      uart[0] = HEADER;
      
      if (portTwo.read() == HEADER) {
        uart[1] = HEADER;

        for(i = 2; i < 9; i++) {
          uart[i] = portTwo.read();
        }

        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
        if (uart[8] == (check&0xff)) {
          distance2 = uart[2] + uart[3] * 256;         

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
    }        
  }  
}

TimedAction Sensor1Thread = TimedAction(1, readSensor1);
TimedAction Sensor2Thread = TimedAction(1, readSensor2);
 
void setup() {
  Serial.begin (9600);  
  pinMode(LED, OUTPUT);  
  portOne.begin(115200);  
  portTwo.begin(115200);
}
 
void loop() {    
  getGateSize();  

  digitalWrite(LED, LOW);        
  noNewTone(BUZZER);  

  if (gate_size_recived) {
    Sensor1Thread.check();
    Sensor2Thread.check();
  }
}
