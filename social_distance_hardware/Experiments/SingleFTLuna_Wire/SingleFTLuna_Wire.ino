
#include <Wire.h>

unsigned char ok_flag;
unsigned char fail_flag;

unsigned short lenth_val = 0;
unsigned char i2c_rx_buf[16];
unsigned char dirsend_flag=0;
//uint8_t addr = 0x52;
uint8_t addr = 0x10;
//uint8_t addr = 0x59;

int dist;
int strength;
int check;
int uart[9];
int i;
const int HEADER=0x59;

byte error;

void setup() {    
  Wire.begin(); 
  Serial.begin(9600);    
    
// 
//
//    // If no error was reported, show the device we found!
//    if (error == 0)
//    {
//      Serial.print("I2C device found at address 0x");
//      if (addr<16)
//        Serial.print("0");
//      Serial.print(addr,HEX);
//      Serial.println(" !");
//      
//    }
//  
//  delay(3000);
}

void SensorRead(unsigned char addr,unsigned char* datbuf,uint8_t  cnt) 
{  
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(addr); // transmit to device #82 (0x52)
  Wire.write(byte(addr));  
  Wire.endTransmission();      // stop transmitting
  // step 2: wait for readings to happen
  delay(1);                   // datasheet suggests at least 30uS
  // step 3: request reading from sensor
  Wire.requestFrom(byte(addr), cnt);    // request cnt bytes from slave device #82 (0x52)
  Serial.println(Wire.read());
  *datbuf++ = Wire.read();  // receive high byte (overwrites previous reading)
  *datbuf++ = Wire.read(); // receive low byte as lower 8 bits  
}

int ReadDistance(){
    SensorRead(HEADER,i2c_rx_buf,2);
    lenth_val=i2c_rx_buf[0];
    lenth_val=lenth_val<<8;
    lenth_val|=i2c_rx_buf[1];
    delay(300); 
    return lenth_val;
}


void loop() {
//  if (portOne.available()) {
//    if (portOne.read() == HEADER) {
//      uart[0] = HEADER;
//      
//      if (portOne.read() == HEADER) {
//        uart[1] = HEADER;
//
//        for(i = 2; i < 9; i++) {
//          uart[i] = portOne.read();
//        }
//
//        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
//        if (uart[8] == (check&0xff)) {
//          dist = uart[2] + uart[3] * 256;
//
//          Serial.print(dist);
//        }
//        
//      }
//      
//    }    
    
//  }

  
//   int x=ReadDistance();
//   
//   Serial.print(x);
//   Serial.println(" mm");

  Wire.beginTransmission(addr);
  error = Wire.endTransmission();

  if (error == 0) {
    Serial.println("caca");  
  }

  delay(100);

}
