#include <SoftI2CMaster.h>

/*
  TOF10120 Distance Sensor Demonstration
  TOF10120-Demo.ino
  Demonstrates use of TOF10120 Distance Sensor
  Adapted from code from SurtrTech
  Displays results on Serial Monitor

  DroneBot Workshop 2019
  https://dronebotworkshop.com
*/


unsigned char ok_flag;
unsigned char fail_flag;

unsigned short lenth_val = 0;
unsigned char i2c_rx_buf[16];
unsigned char dirsend_flag=0;
uint8_t addr = 0x52;

const int sdaPin_1 = 2; // Arduino GPIO 4 is physical pin 7 of ATtiny841.
const int sclPin_1 = 3; // Arduino GPIO 6 is physical pin 9 of ATtiny841.

const int sdaPin_2 = 4; // Arduino GPIO 4 is physical pin 7 of ATtiny841.
const int sclPin_2 = 5; // Arduino GPIO 6 is physical pin 9 of ATtiny841.

SoftI2CMaster i2c_1 = SoftI2CMaster( sclPin_1, sdaPin_1, 0 );
SoftI2CMaster i2c_2 = SoftI2CMaster( sclPin_2, sdaPin_2, 0 );

byte error;

void setup() {  
  delay(3000);
  Serial.begin(9600,SERIAL_8N1);   
  printf_begin();          
  
  error = i2c_1.beginTransmission(addr);
    i2c_1.endTransmission();

    // If no error was reported, show the device we found!
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (addr<16)
        Serial.print("0");
      Serial.print(addr,HEX);
      Serial.println(" !");
 
      
    }

  error = i2c_2.beginTransmission(addr);
    i2c_2.endTransmission();

    // If no error was reported, show the device we found!
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (addr<16)
        Serial.print("0");
      Serial.print(addr,HEX);
      Serial.println(" !");
 
      
    }    
  delay(3000);
}

void loop() {

   int x=ReadDistance(i2c_1);
   int x2=ReadDistance(i2c_2);   

   if (x >= 0 && x <=1800) {
     Serial.print(x);
     Serial.print(" mm -");
   }   

   if (x2 >= 0 && x2 <=1800) {
     Serial.print(x2);
     Serial.println(" mm");
   }
  
}

int serial_putc( char c, struct __file * )
{
  Serial.write( c );
  return c;
}

void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}



void SensorRead(SoftI2CMaster ic, unsigned char* datbuf,uint8_t  cnt) 
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
    delay(100); 
    return lenth_val;
}
