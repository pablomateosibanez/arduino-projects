#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11);
SoftwareSerial mySerial2(12, 13);


int dist = 0;
int check;
int uart[9];
int i;
const int HEADER=0x59;

void setup() {
  Serial.begin(9600);
  mySerial.begin(115200);  
 // mySerial2.begin(115200);    
}

int processSensor(SoftwareSerial &pSerial) {  
  if (pSerial.available()) 
  {    
    if (pSerial.read() == HEADER)
    {
      uart[0] = HEADER;

      if (pSerial.read() == HEADER)
      {
        uart[1] = HEADER;
        for (i = 2; i < 9; i++) 
        {
          uart[i] = pSerial.read();
        }

        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];

        if (uart[8] == (check & 0xff))
        {
          dist = uart[2] + uart[3] * 256;                                
          return dist;
        }        
      }
    }    
  }

  
}

void loop() { 
  Serial.println(processSensor(mySerial));  

//  Serial.print(" - ");
//
  //Serial.println(processSensor(mySerial2));  
  

}
