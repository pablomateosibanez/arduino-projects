#include "NewPing.h"
#include <NewTone.h>

// Hook up HC-SR04 with Trig to Arduino Pin 10, Echo to Arduino pin 13
// Maximum Distance is 400 cm

#define TRIGGER_PIN_1   8
#define ECHO_PIN_1      9
#define TRIGGER_PIN_2  11
#define ECHO_PIN_2     10
#define BUZZER          7
#define MAX_DISTANCE   90
#define LED            13
const int touchPin = 7;

float duration;
float distance1;
float distance2;
int capacity_count = 0; 
String capacity_count_s;
int inside_count = 0;
int iterations = 1; 
 
NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE);

void music1() {
    NewTone(BUZZER, 1046.50);
    delay(100);
    noNewTone(BUZZER);
    delay(100);
    NewTone(BUZZER, 1046.50);
    delay(100);
    noNewTone(BUZZER);   
}

void music2() {
    NewTone(BUZZER, 130.81);
    delay(100);
    noNewTone(BUZZER);
    delay(100);
    NewTone(BUZZER, 130.81);
    delay(100);
    noNewTone(BUZZER);   
}
 
void setup() {
  Serial.begin (9600);  
  pinMode(LED, OUTPUT);  
  
  //delay(25000);    
}
 
void loop() {
  //distance = sonar.ping_cm();
   //duration = sonar.ping();
  //duration = sonar.ping_median(iterations);
  
  // Determine distance from duration
  // Use 343 metres per second as speed of sound  
  //distance = (duration / 2) * 0.0343;

  distance1 = sonar1.ping_cm(); 
  distance2 = sonar2.ping_cm();     

//  Serial.println("Distance_1: " + String(distance1) + " " + "Distance_2: " + String(distance2));

  if (distance1 >= MAX_DISTANCE || distance1 <= 2) { 
    digitalWrite(LED, LOW);        
    noNewTone(BUZZER);
  }
  else { 
    capacity_count++;    
    inside_count++;
    capacity_count_s = (String)capacity_count;
    //Serial.print(capacity_count_s);
    Serial.print("I");    
    
    digitalWrite(LED, HIGH);        

    music1();
    
//    Serial.print("Distance_1 = ");    
//    Serial.print(distance1);
//    Serial.println(" cm");    
    delay(1000);
  } 

  
  if (distance2 >= MAX_DISTANCE || distance2 <= 2) {
    digitalWrite(LED, LOW);            
    noNewTone(BUZZER);
  }
  else {
    capacity_count--;
    capacity_count_s = (String)capacity_count;
    //Serial.print(capacity_count_s);
    Serial.print("D");
    
    digitalWrite(LED, HIGH);            
    
    music2();
    
//    Serial.print("Distance_2 = ");    
//    Serial.print(distance2);
//    Serial.println(" cm");
    delay(1000);
  }  


  //Serial.println(capacity_count);
  
  delay(100);
}
