#include <TimedAction.h>
#include <NewTone.h>
#include <SoftwareSerial.h>

#define LED 13
#define BUZZER 7
#define MAX_DISTANCE_MM 1300

int c1 = 0;
int c2 = 0;

float duration;
int distance1;
int distance2;
int capacity_count = 0;  
String capacity_count_s;
int inside_count = 0;
int iterations = 1; 

// TOF1020 UART
String rx_a = "";
String rx_b = "";
bool check_a = false;
bool check_b = false;

char rx_char;
uint8_t double_m = 0;
uint16_t last_a = 0;
uint16_t last_b = 0;
bool newLine_a = false;
bool newLine_b = false;
uint8_t errCheck_a = 0;
uint8_t errCheck_b = 0;
uint16_t keep_a = 0;
uint16_t keep_b = 0; 

SoftwareSerial portOne(3, 2);
SoftwareSerial portTwo(5, 4);

void music1() {
    NewTone(BUZZER, 3046.50);
    delay(50);
    noNewTone(BUZZER);
//    delay(10);
//    NewTone(BUZZER, 1046.50);
//    delay(10);
//    noNewTone(BUZZER);   
}

void music2() {
    NewTone(BUZZER, 130.81);
    delay(50);
    noNewTone(BUZZER);
//    delay(10);
//    NewTone(BUZZER, 130.81);
//    delay(10);
//    noNewTone(BUZZER);   
}

int getDistanceUART() {  
  rx_a = "";  
  unsigned long time =millis(); 
  portOne.listen();
  delay(100);
  
  while(1){
    if(portOne.available() > 0){
      rx_char = portOne.read();
      if(rx_char == 10){
        newLine_a = true;
        continue;
      }
      if(newLine_a == true){
        if(rx_char == 'm'){
          double_m = double_m + 1;  
        }
        else{
          rx_a.concat(rx_char);
        }
        if(double_m == 2){
          newLine_a = false;
          check_a = true;
          break;
        }
      }else{
        errCheck_a++;
      }
      
    }else{
      if( (errCheck_a++) > 100){
        break;  
      }
    }
  }
  newLine_a = false;
  errCheck_a = 0;
  
  if(check_a==false){
    rx_a="";
  }else{
    if( (rx_a.toInt()-last_a>50) || (rx_a.toInt()-last_a<-50) ){
      if(rx_a != ""){
        last_a = rx_a.toInt();
      }
      rx_a="";     
    }else{
      if(rx_a != ""){
        last_a = rx_a.toInt();
      }
    }
  }
  
  check_a = false;
  double_m = 0;
  if(portOne.isListening() == 1){
    portOne.stopListening();
  }

  if(rx_a != ""){
    keep_a = rx_a.toInt();
  }  
  Serial.println(millis()-time);
  distance1 = keep_a;
  return keep_a;
}

int getDistanceUART2() {
  rx_b = "";    
  
  portTwo.listen();
  delay(100);
  
  while(1){
    if(portTwo.available() > 0){
      rx_char = portTwo.read();
      if(rx_char == 10){
        newLine_b = true;
        continue;
      }
      if(newLine_b == true){
        if(rx_char == 'm'){
          double_m = double_m + 1;  
        }
        else{
          rx_b.concat(rx_char);
        }
        if(double_m == 2){
          newLine_b = false;
          check_b = true;
          break;
        }
      }else{
        errCheck_b++;
      }
      
    }else{
      if( (errCheck_b++) > 100){
        break;  
      }
    }
  }
  newLine_b = false;
  errCheck_b = 0;
  
  if(check_b == false){
    rx_b="";
  }else{
    if( (rx_b.toInt()-last_b>50) || (rx_b.toInt()-last_b<-50) ){
      if(rx_b != ""){
        last_b = rx_b.toInt();
      }
      rx_b="";     
    }else{
      if(rx_b != ""){
        last_b = rx_b.toInt();
      }
    }
  }
  
  check_b = false;
  double_m = 0;
  if(portTwo.isListening() == 1){
    portTwo.stopListening();
  }

  if(rx_b != ""){
    keep_b = rx_b.toInt();
  }

  distance2 = keep_b;
  return keep_b;
}

 
void setup() { 
  portOne.begin(9600);
  portTwo.begin(9600);  
//  delay(22);
//  portOne.write("s2-20#");
//  portTwo.write("s2-20#");

  Serial.begin(9600);  

  pinMode(LED, OUTPUT);  
}

void checkSensor1() {
  distance1 = getDistanceUART();
  
  if (distance1 >= MAX_DISTANCE_MM || distance1 <= 2) { 
    c1 = 0;            
    digitalWrite(LED, LOW);        
    noNewTone(BUZZER);
  }
  else { 
    if (c1 < 1) {
      c1++;
//      capacity_count++;    
//      inside_count++;
//      capacity_count_s = (String)capacity_count;
      //Serial.print(capacity_count_s);
      Serial.print("I");    
      
      digitalWrite(LED, HIGH);        
  
      music1();
      
  //    Serial.print("Distance_1 = ");    
  //    Serial.print(distance1);
  //    Serial.println(" cm");    
      delay(950);
    }
  }   
}

void checkSensor2() {
  distance2 = getDistanceUART2();     
  
  if (distance2 >= MAX_DISTANCE_MM || distance2 <= 2) {
    c2 = 0;                
    digitalWrite(LED, LOW);            
    noNewTone(BUZZER);
  }
  else {
    if (c2 < 1) {    
      c2++;
//      capacity_count--;
//      capacity_count_s = (String)capacity_count;
      //Serial.print(capacity_count_s);
      Serial.print("D");
      
      digitalWrite(LED, HIGH);            
      
      music2();
      
  //    Serial.print("Distance_2 = ");    
  //    Serial.print(distance2);
  //    Serial.println(" cm");
      delay(950);
    }
  }  
}

//TimedAction sensor1Thread = TimedAction(50,getDistanceUART());
//TimedAction sensor2Thread = TimedAction(50,getDistanceUART2());
 
void loop() {  
  checkSensor1();
  checkSensor2(); 
  
//
//  Serial.print("A: ");
//  Serial.print(distance1);  
//
//  Serial.print("  ");
//  Serial.print("B: ");
//  Serial.println(distance2);

//  sensor1Thread.check();
//  sensor2Thread.check();   
}
