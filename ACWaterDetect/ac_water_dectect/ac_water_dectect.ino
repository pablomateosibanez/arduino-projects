#include <NewTone.h>

#define BUZZER 12
int resval = 0;
int respin = A5;

void setup() {
  Serial.begin(9600);
}

void in_sound() {
    NewTone(BUZZER, 800);
    delay(150);
    noNewTone(BUZZER);
}

void loop() {

  resval = analogRead(respin);

  Serial.println(resval);    
  if (resval > 600) {
    in_sound();    
  }  

  delay(100);
}
