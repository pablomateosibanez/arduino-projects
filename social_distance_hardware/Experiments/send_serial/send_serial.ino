bool send_gate_size = false;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

}


void sendGateSize() {
  //if (send_gate_size == false) {
    Serial1.print("GATE:" + String(10));    

//    if (Serial.available()) 
//      Serial.println("*" + Serial.readString());

    send_gate_size = true;
  //}  
}

void loop() {
  sendGateSize();  
}
