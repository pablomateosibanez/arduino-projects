int LED = 13;
char buff [50];
volatile byte indx;

int MAX_DISTANCE_TO_DETECTED = 100;
//int MAX_DISTANCE_TO_DETECTED = 600;
#define MAX_DISTANCE_SENSOR 800
String inputString = "";
bool gate_size_recived = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LED, OUTPUT);    
}


void getGateSize() {    
  int gate_size = 0;  
  //if (gate_size_recived == false) {      
    if (Serial1.available()) {                    
      inputString = Serial1.readString();              
      if (inputString.length() != 0) {     
        digitalWrite(LED, HIGH);
            delay(1000);     
            digitalWrite(LED, LOW);       
            delay(1000);              
        String G = inputString.substring(0, 5);        
        if (G == "GATE:") {
          String N = inputString.substring(5, inputString.length());                    
          gate_size = N.toInt();
          if (gate_size > 0) {                        
            gate_size_recived = true;   
            MAX_DISTANCE_TO_DETECTED = gate_size;        
            digitalWrite(LED, HIGH);
            delay(1000);     
            digitalWrite(LED, LOW);       
            delay(1000);                 
            //Serial.print("FGATE:");
          }        
        }
      }
    }  
  //}  
}

void loop() {
  getGateSize();
  
}
