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

#include <TimedAction.h>
#include <DFRobot_TFmini.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial1(2, 3); // RX, TX
SoftwareSerial mySerial2(5, 4); // RX, TX

DFRobot_TFmini  TFmini1;
DFRobot_TFmini  TFmini2;

uint16_t distance,strength;
#define LED 13
int check;
int uart[9];
int i;
const int HEADER=0x59;

//void readSensor1() {
//  mySerial.listen();  
//  if (mySerial.available()) {
//    if (mySerial.read() == HEADER) {
//      uart[0] = HEADER;
//      
//      if (mySerial.read() == HEADER) {
//        uart[1] = HEADER;
//
//        for(i = 2; i < 9; i++) {
//          uart[i] = mySerial.read();
//        }
//
//        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
//        if (uart[8] == (check&0xff)) {
//          distance = uart[2] + uart[3] * 256;          
//          Serial.println(distance);
//          if (distance > 0 && distance <= 130) {
//            digitalWrite(LED, HIGH);                    
//          }
//          else
//          {
//            digitalWrite(LED, LOW);
//          }                    
//        }        
//      }      
//    }        
//  }  
//}

void readSensorTFMini_1() {
  if(TFmini1.measure()){                      //Measure Distance and get signal strength
        distance = TFmini1.getDistance();       //Get distance data
        strength = TFmini1.getStrength();       //Get signal strength data
        Serial.print("Distance = ");
        if (distance > 0 && distance <= 130) {
          digitalWrite(LED, HIGH);                    
        }
        else
        {
          digitalWrite(LED, LOW);
        }
          
        Serial.print(distance);
        Serial.println("mm");
        Serial.print("Strength = ");
        Serial.println(strength);
        
    }
}

void readSensorTFMini_2() {
  if(TFmini2.measure()){                      //Measure Distance and get signal strength
        distance = TFmini2.getDistance();       //Get distance data
        strength = TFmini2.getStrength();       //Get signal strength data
        Serial.print("Distance = ");
        if (distance > 0 && distance <= 130) {
          digitalWrite(LED, HIGH);                    
        }
        else
        {
          digitalWrite(LED, LOW);
        }
          
        Serial.print(distance);
        Serial.println("mm");
        Serial.print("Strength = ");
        Serial.println(strength);
        
    }
}


void setup(){
    pinMode(LED, OUTPUT);    
    Serial.begin(115200);
    //mySerial.begin(115200);        
    //mySerial2.begin(115200);            
    TFmini1.begin(mySerial1);    
    //TFmini2.begin(mySerial2);
}

TimedAction Sensor1Thread = TimedAction(1, readSensorTFMini_1);
TimedAction Sensor2Thread = TimedAction(1, readSensorTFMini_2);

void loop(){
//  digitalWrite(LED, HIGH);                    
//  delay(500);
//  digitalWrite(LED, LOW);                    
//  delay(500);
  Sensor1Thread.check();
  //Sensor2Thread.check();
  
  //readSensorTFMini();  
  //readSensor1();  
    
}
