/* File Name: TFLunaI2C_example.ino

 *
 * There are only two important functions: 'getData' and 'sendCommand'
 *
 *  NOTE: By default the I2C device address is set to 0x10. If you need
 *  to address multiple devices or need to change the default address for
 *  any reason, your code must thereafter include an unsigned, 8-bit
 *  'addr' value at the end of every call to 'getData()' or 'sendCommand()'.
 *  
 *  NOTE: This library also includes a simple 'getData( dist)' function that
 *  passes back distance data only. It assumes use of the default I2C address.
 *
 */

#include <Wire.h>     // Arduino standard I2C/Two-Wire Library
#include "printf.h"   // Modified by to support Intel based Arduino
                      // devices such as the Galileo. Download from:
                      // https://github.com/spaniakos/AES/blob/master/printf.h

#include "TFLunaI2C.h"
TFLunaI2C tf_luna;         // Create a TFLuna I2C object

void setup()
{
    Wire.begin();            // Initialize two-wire interface
//    Wire.setClock( 400000);  // Set I2C bus speed to 'Fast' if
                               // your Arduino supports 400KHz.
    Serial.begin( 115200);   // Initialize terminal serial port
    printf_begin();          // Initialize printf library.
    delay(20);

    printf( "\r\nTFLuna I2C Library Example\r\n");  // say 'hello'

    // Send some example commands to the TFLuna
    // - - - - - - - - - - - - - - - - - - - - - - - -
    printf( "System reset: ");
    if( tf_luna.sendCommand( SYSTEM_RESET, 0))
    {
        printf( "passed.\r\n");
    }
    else tf_luna.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -
    printf( "Firmware version: ");
    if( tf_luna.sendCommand( OBTAIN_FIRMWARE_VERSION, 0))
    {
        printf( "%1u.", tf_luna.version[ 0]); // print three single numbers
        printf( "%1u.", tf_luna.version[ 1]); // each separated by a dot
        printf( "%1u\r\n", tf_luna.version[ 2]);
    }
    else tf_luna.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -
    printf( "Data-Frame rate: ");
    if( tf_luna.sendCommand( SET_FRAME_RATE, FRAME_250))
    {
        printf( "%2uHz.\r\n", FRAME_250);
    }
    else tf_luna.printErrorStatus();
    // - - - - - - - - - - - - - - - - - - - - - - - -

    delay(500);            // And wait for half a second.
}

// Initialize variables
uint16_t tfDist = 0;       // Distance to object in centimeters
uint16_t tfFlux = 0;       // Signal strength or quality of return signal
uint16_t tfTemp = 0;       // Internal temperature of Lidar sensor chip

// = = = = = = = = = =  MAIN LOOP  = = = = = = = = = =
void loop()
{
    delay(50);    //  Run loop at approximately 20Hz.

    tf_luna.getData( tfDist, tfFlux, tfTemp); // Get a frame of data
    if( tf_luna.status == TFMP_READY)         // If no error...
    {
        printf( "Dist:%04ucm ", tfDist);   // display distance,
        printf( "Flux:%05u ",   tfFlux);   // display signal strength/quality,
        printf( "Temp:%2u\°C",  tfTemp);   // display temperature,
        printf( "\r\n");                   // end-of-line.
    }
    else tf_luna.printErrorStatus();          // Otherwise, display error. 
}
// = = = = = = = = =  End of Main Loop  = = = = = = = = =
