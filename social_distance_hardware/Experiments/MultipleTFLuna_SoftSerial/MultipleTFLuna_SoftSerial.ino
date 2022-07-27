#include <TimedAction.h>
#include <NewTone.h>
#include <SoftwareSerial.h>

#define BUZZER 12
#define LED 13

int dist;
int dist2;
int strength;
int check;
int uart[9];
int uart2[9];
int i;
const int HEADER=0x59;

// software serial #1: RX = digital pin 10, TX = digital pin 11
SoftwareSerial portOne(2, 3);

// software serial #2: RX = digital pin 8, TX = digital pin 9
// on the Mega, use other pins instead, since 8 and 9 don't work on the Mega
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


void setup() {
  // Open serial communications and wait for port to open:
  pinMode(LED, OUTPUT);    
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  // Start each software serial port
  portOne.begin(115200);
  portTwo.begin(115200);
}

void Sensor1() {
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
          dist = uart[2] + uart[3] * 256;

          digitalWrite(LED, LOW);
          if (dist >= 0 && dist <= 50) {
            digitalWrite(LED, HIGH);
            in_sound();            
            delay(1000); 
          }
        }        
      }      
    }        
  }
}

void Sensor2() {
portTwo.listen();  
  if (portTwo.available()) {
    if (portTwo.read() == HEADER) {
      uart2[0] = HEADER;
      
      if (portTwo.read() == HEADER) {
        uart2[1] = HEADER;

        for(i = 2; i < 9; i++) {
          uart2[i] = portTwo.read();
        }

        check = uart2[0] + uart2[1] + uart2[2] + uart2[3] + uart2[4] + uart2[5] + uart2[6] + uart2[7];
        if (uart2[8] == (check&0xff)) {
          dist2 = uart2[2] + uart2[3] * 256;
          
          digitalWrite(LED, LOW);
          if (dist2 >= 0 && dist2 <= 50) {
            digitalWrite(LED, HIGH);
            out_sound();            
            delay(1000); 
          }
        }
        
      }
      
    }    
    
  }
  
}

TimedAction Sensor1Thread = TimedAction(1,Sensor1);
TimedAction Sensor2Thread = TimedAction(1,Sensor2);

void loop() {

  
  
  Sensor1Thread.check();
  Sensor2Thread.check();

  

  
}
