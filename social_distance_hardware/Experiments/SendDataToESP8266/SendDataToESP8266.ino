uint16_t count = 0;
int led = 13;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(115200);
  Serial.begin(9600);  
  pinMode(led, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  count++;  

  
  String count_s = (String)count;
  digitalWrite(led, HIGH);
  delay(500);
  digitalWrite(led, LOW);
  //Serial.print("Update capacity count:");
  Serial.print(count_s);

  //Serial.println("Sent:  " + outMessage);
//  char* CharString;                                    //
//  count_s.toCharArray(CharString, count_s.length()); // My Changes Are Here
//  Serial.write(CharString); 
//  
  //Serial.write(count_s);  
  //Serial.write(count);
  //Serial.write("\n");
    
  delay(1000);  
}
