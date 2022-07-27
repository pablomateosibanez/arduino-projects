#include <SoftwareSerial.h>
#include <TFMPlus.h>  // Include TFMini Plus Library v1.4.0

// UART SENSOR_2
int16_t distance2 = 0;
int16_t tfFlux2 = 0;
int16_t tfTemp2 = 0;
TFMPlus tfmP;
SoftwareSerial mySerial(10, 11);

// SENSOR_2
void setupUARTSensor() {
  //Serial1.begin(9600);  
  mySerial.begin(9600);
  delay(20);
  tfmP.begin(&mySerial);   // Initialize device library object and...
                           // pass device serial port to the object.
  
  // Send some example commands to the TFMini-Plus
  // - - Perform a system reset - - - - - - - - - - -
  
  Serial.println( "System reset: ");
  if( tfmP.sendCommand( SYSTEM_RESET, 0))
  {
      Serial.println( "passed.");
  }
  else tfmP.printReply();
  
  delay(300);  // added to allow the System Rest enough time to complete
  
  // - - Display the firmware version - - - - - - - - -
  
  Serial.println( "Firmware version: ");
  if( tfmP.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
  {
      Serial.println(tfmP.version[ 0]); // print three single numbers
      Serial.println(tfmP.version[ 1]); // each separated by a dot
      Serial.println(tfmP.version[ 2]);
  }
  else tfmP.printReply();
  // - - Set the data frame-rate to 20Hz - - - - - - - -
  
  Serial.println( "Data-Frame rate: ");
  if( tfmP.sendCommand( SET_FRAME_RATE, FRAME_20))
  {
      Serial.println(FRAME_20);
  }
  else tfmP.printReply();
  // - - - - - - - - - - - - - - - - - - - - - - - -
  
  delay(500);            // And wait for half a second.  
}

void setup() {
  Serial.begin(9600);        
  delay(20);

  setupUARTSensor();
}

void loop() {
  tfmP.getData(distance2, tfFlux2, tfTemp2);
  Serial.println(distance2);
}
