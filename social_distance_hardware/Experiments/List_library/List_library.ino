#include "ListLib.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  // Crear una nueva lista
  List<String> list;


  String value;
  String op;

  op = "1:";
  value = "100";   
  list.Add(op + value);

  op = "1:";
  value = "0";   
  list.Add(op + value);

  op = "2:";
  value = "100";   
  list.Add(op + value);

  op = "2:";
  value = "0";   
  list.Add(op + value);

  for (int i = 0; i < list.Count(); i++)
  {
    Serial.println(list[i]);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
