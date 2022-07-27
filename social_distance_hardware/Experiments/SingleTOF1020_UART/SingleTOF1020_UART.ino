#include <SoftwareSerial.h>

String rx_a = "";
bool check_a = false;
char rx_char;
uint8_t double_m = 0;
uint16_t last_a = 0;
bool newLine_a = false;
uint8_t errCheck_a = 0;
uint16_t keep_a = 0;

// software serial #1: RX = digital pin 10, TX = digital pin 11
SoftwareSerial portOne(3, 2);


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  portOne.begin(9600);  
  Serial.println("start..");
  delay(22);
  portOne.write("s2-20#");  
}

int getDistanceUART() {
  rx_a = "";  
  
  portOne.listen();
  delay(40);
  
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
    Serial.print("A: ");
    Serial.println(keep_a);
  }  

  return keep_a;
}

void loop() {
  
}
