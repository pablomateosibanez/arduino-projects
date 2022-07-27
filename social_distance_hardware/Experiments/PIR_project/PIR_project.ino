#define BUZZER 12
const int LED = 13;
const int SENSOR_1 = 4;
const int SENSOR_2 = 2;
int pirState_1 = LOW;           // de inicio no hay movimiento
int pirState_2 = LOW;           // de inicio no hay movimiento
int value_1 = 0;    
int value_2 = 0;    
int val = 0;
int inside_count;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(SENSOR_1, INPUT);
  pinMode(SENSOR_2, INPUT);  
  inside_count = 0;
}

void loop() {
// ************* SENSOR 1 ************* //  
  
  value_1 = digitalRead(SENSOR_1);
  if (pirState_2 == LOW) {  
    if (value_1 == HIGH)   //si está activado
    { 
        digitalWrite(LED, HIGH);  //LED ON
        if (pirState_1 == LOW)  //si previamente estaba apagado
        {
          inside_count++;
          //tone(BUZZER, 440);            
          delay(1000);
          noTone(BUZZER);            
          Serial.println("Sensor 1 activado");
          pirState_1 = HIGH;
        }
    } 
    else   //si esta desactivado
    {
        digitalWrite(LED, LOW); // LED OFF
        if (pirState_1 == HIGH)  //si previamente estaba encendido
        {
          Serial.println("Sensor 1 parado");
          pirState_1 = LOW;
        }      
    }
  }
  


// ************* SENSOR 2 ************* //      
  value_2 = digitalRead(SENSOR_2);

  if (pirState_1 == LOW) {  
    if (value_2 == HIGH)   //si está activado
    { 
        digitalWrite(LED, HIGH);  //LED ON
        if (pirState_2 == LOW)  //si previamente estaba apagado
        {
          inside_count--;
          //tone(BUZZER, 440);                    
          delay(1000);        
          noTone(BUZZER);                    
          Serial.println("Sensor 2 activado");
          pirState_2 = HIGH;
        }
    } 
    else   //si esta desactivado
    {
        digitalWrite(LED, LOW); // LED OFF
        if (pirState_2 == HIGH)  //si previamente estaba encendido
        {
          Serial.println("Sensor 2 parado");
          pirState_2 = LOW;
        }
    }        
  }
  
  
  
  Serial.println(inside_count);

  delay(100);
}
