#include <SoftI2CMaster.h>
#include <NewTone.h>

#define BUZZER 12
#define LED 13
const int sdaPin_1 = 2; 
const int sclPin_1 = 3; 
const int sdaPin_2 = 4; 
const int sclPin_2 = 5; 

int MAX_DISTANCE_TO_DETECTED = 500;
#define MAX_DISTANCE_SENSOR 1800
float distance1;
float distance2;
bool trigger_1 = false;
bool trigger_2 = false;
String inputString = "";
bool gate_size_recived = false;
int waitTimeToDetected = 1200;

// I2C
unsigned char ok_flag;
unsigned char fail_flag;

unsigned short lenth_val = 0;
unsigned char i2c_rx_buf[16];
unsigned char dirsend_flag=0;
uint8_t addr = 0x52;
byte error;

SoftI2CMaster i2c_1 = SoftI2CMaster(sclPin_1, sdaPin_1, 0);
SoftI2CMaster i2c_2 = SoftI2CMaster(sclPin_2, sdaPin_2, 0); 

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

void SensorRead(SoftI2CMaster ic, unsigned char* datbuf, uint8_t cnt) 
{  
  // step 1: instruct sensor to read echoes
  error = ic.beginTransmission(addr); // transmit to device #82 (0x52)
  ic.write(byte(addr));  
  ic.endTransmission();      // stop transmitting
  // step 2: wait for readings to happen
  delay(1);                   // datasheet suggests at least 30uS
  // step 3: request reading from sensor
  ic.requestFrom(byte(addr), cnt);    // request cnt bytes from slave device #82 (0x52)
  *datbuf++ = ic.read();  // receive high byte (overwrites previous reading)
  *datbuf++ = ic.read(); // receive low byte as lower 8 bits  
}

int ReadDistance(SoftI2CMaster ic){
    SensorRead(ic, i2c_rx_buf,2);
    lenth_val=i2c_rx_buf[0];
    lenth_val=lenth_val<<8;
    lenth_val|=i2c_rx_buf[1];
    delay(1); 
    return lenth_val;
}

void getGateSize() {
  int gate_size = 0;
  if (gate_size_recived == false) {
    while (Serial.available()) {
      inputString = Serial.readString();                
      if (inputString.length() != 0) {        
        gate_size = inputString.toInt();
        if (gate_size > 0) {
          gate_size_recived = true;   
          MAX_DISTANCE_TO_DETECTED = gate_size;             
        }
//        for (int I = 0; I < gate_size; I++) {        
//          digitalWrite(LED, HIGH);
//          Serial.println(inputString);      
//          delay(100);
//          digitalWrite(LED, LOW);            
//          delay(100);          
//        }
      }
    }  
  }  
}
 
void setup() {
  Serial.begin (9600);  
  pinMode(LED, OUTPUT);  
}
 
void loop() {    
  getGateSize();
  
  distance1 = ReadDistance(i2c_1);
  distance2 = ReadDistance(i2c_2);  

// ############### normal functionality #####################
  /*if (distance1 >= MAX_DISTANCE_TO_DETECTED || distance1 <= 2) { 
    digitalWrite(LED, LOW);        
    noNewTone(BUZZER);
  }
  else {     
    Serial.print("I");              
    digitalWrite(LED, HIGH);
    in_sound();
    delay(1000);
  } 

  if (distance2 >= MAX_DISTANCE_TO_DETECTED || distance2 <= 2) { 
    digitalWrite(LED, LOW);        
    noNewTone(BUZZER);
  }
  else {     
    Serial.print("D");              
    digitalWrite(LED, HIGH);
    out_sound();
    delay(1000);
  } */ 


  digitalWrite(LED, LOW);        
  noNewTone(BUZZER);

  if (distance1 >= 0 && distance1 <= MAX_DISTANCE_SENSOR) {
    if (distance1 <= MAX_DISTANCE_TO_DETECTED) {    
      Serial.print("I");              
      digitalWrite(LED, HIGH);
      in_sound();
      delay(waitTimeToDetected);            
    }       
  }

  if (distance2 >= 0 && distance2 <= MAX_DISTANCE_SENSOR) {
    if (distance2 <= MAX_DISTANCE_TO_DETECTED) {    
      Serial.print("D");              
      digitalWrite(LED, HIGH);
      out_sound();
      delay(waitTimeToDetected);            
    }
  }  
  
  
  delay(1);   
}
