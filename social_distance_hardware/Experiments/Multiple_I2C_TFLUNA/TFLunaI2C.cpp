/* File Name: TFLunaI2C.cpp
 * Described: Arduino Library for the Benewake TFMini-Luna Lidar sensor
 *            configured for the I2C interface
 *
 * Default settings for the TFLuna I2C are:
 *    0x10  -  slave device address
 *    100Mhz - bus clock speed
 *    100Hz  - data frame-rate
 *    Centimeter - distance measurement format
 *    Celsius - temperature measurement scale
 *
 *  There are only two important functions: 'getData' and 'sendCommand'
 *
 *  NOTE: By default the I2C device address is set to 0x10. If you need
 *  to address multiple devices or need to change the default address for
 *  any reason, your code must thereafter include an unsigned, 8-bit
 *  'addr' value at the end of every call to 'getData()' or 'sendCommand()'.
 *
 *  'getData( dist, flux, temp, addr)' passes back measurement values in
 *  three unsigned, 16-bit variables:
 *     dist - distance to target in centimeters: 10cm - 1200cm
 *     flux - strength, voltage or quality of returned signal
 *            in arbitrary units: 0 - 65535Z
 *     temp - chip temperature in degrees Celsius: -25°C to 125°C
 *  and sends;
 *     addr - optional unsigned 8-bit address value.
 *  - If default device address is used unchanged, the 'addr' value may
 *  be omitted.  Otherwise, a correct 'addr' value always must be sent.
 *  - If the function completes without error, it returns 'True' and sets
 *  a public, one-byte 'status' code to zero.  Otherwise. it returns
 *  'False' and sets the 'status' code to a library defined error code.
 *  
 *  NOTE: This library also includes a simple 'getData( dist)' function that
 *  passes back distance data only. It assumes use of the default I2C address.
 *
 * 'sendCommand( cmnd, param, addr)'
 *  The function sends an unsigned 32-bit command and an unsigned 32-bit
 *  parameter value plus an optional, unsigned, 8-bit I2C device address.
 *  If the function completes without error it returns 'True' and sets
 *  a public one-byte 'status' code to zero.  Otherwise it returns 'False'
 *  and sets the 'status' code to a library defined error code.
 */

#include "TFLunaI2C.h"       //  TFLuna I2C library header
#include <Wire.h>          //  Arduino I2C/Two-Wire Library

// Constructor/Destructor
TFLunaI2C::TFLunaI2C(){}
TFLunaI2C::~TFLunaI2C(){}

// = = = = =  GET A FRAME OF DATA FROM THE DEVICE  = = = = = = = = = =
//
bool TFLunaI2C::getData( uint16_t &dist, uint16_t &flux, uint16_t &temp, uint8_t addr)
{
    status = TFMP_READY;    // clear status of any error condition

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 0 - Command device to ready distance data in centimeters
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // The device can also return data in millimeters, but its
    // resolution is only 5mm (o.5cm) and its accuracy is ±5cm.
    if( sendCommand( I2C_FORMAT_CM, 0, addr) != true) return false;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 1 - Get data from the device.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Request one data-frame from the slave device address
    // and keep the I2C interface open.
    Wire.requestFrom( (int)addr, TFMP_FRAME_SIZE, 1);

    memset( frame, 0, sizeof( frame));     // Clear the data-frame buffer.
    for( uint8_t i = 0; i < TFMP_FRAME_SIZE; i++)
    {
      if( Wire.peek() == -1)     // If there is no next byte...
      {
        status = TFMP_I2CREAD;   // then set error...
        Wire.write( 0);          // Put a zero in the xmit buffer.
        Wire.endTransmission();  // Send and Close the I2C interface.
        return false;            // and return "false."
      }
      // Wire.read() returns a signed integer type
      // and advances the rxBufferIndex.
      else frame[ i] = uint8_t( Wire.read());
    }
    Wire.write( 0);          // Put a zero in the xmit buffer.
    Wire.endTransmission();  // Send and Close the I2C interface.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 2 - Perform a checksum test.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Clear the 'chkSum' variable declared in 'TFLunaI2C.h'
    chkSum = 0;
    // Add together all bytes but the last.
    for( uint8_t i = 0; i < ( TFMP_FRAME_SIZE - 1); i++) chkSum += frame[ i];
    //  If the low order byte does not equal the last byte...
    if( ( uint8_t)chkSum != frame[ TFMP_FRAME_SIZE - 1])
    {
      status = TFMP_CHECKSUM;  // then set error...
      return false;            // and return "false."
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 3 - Reorder the frame data and go home
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    dist = frame[ 2] + ( frame[ 3] << 8);
    flux = frame[ 4] + ( frame[ 5] << 8);
    temp = frame[ 6] + ( frame[ 7] << 8);
    // convert temp code to degrees Celsius
    temp = ( temp >> 3) - 256;
    // convert temp to degrees Farenheit
    //if( scale == TFMP_FAREN)  temp = uint8_t( temp * 9 / 5) + 32;

    return true;
}

// Pass back data using default I2C address.
bool TFLunaI2C::getData( uint16_t &dist, uint16_t &flux, uint16_t &temp)
{
  return getData( dist, flux, temp, TFMP_DEFAULT_ADDRESS);
}

// Pass back only the distance data using derfault I2C address.
bool TFLunaI2C::getData( uint16_t &dist)
{
  static uint16_t flux, temp;
  return getData( dist, flux, temp, TFMP_DEFAULT_ADDRESS);
}
//
// - - - - - - End of Get a Frame of Data  - - - - - - - - - -


// = = = = =  SEND A COMMAND TO THE DEVICE  = = = = = = = = = =
//
// Create a proper command byte array, send the command,
// get a response, and return the status.
bool TFLunaI2C::sendCommand( uint32_t cmnd, uint32_t param, uint8_t addr)
{
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 1 - Build the command data to send to the device
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    static uint8_t cmndLen;             // Length of command
    static uint8_t replyLen;            // Length of command reply data
    static uint8_t cmndData[ 8];        // 8 byte send command array

    memset( cmndData, 0, 8);            // Clear the send command array.
    memcpy( &cmndData[ 0], &cmnd, 4);   // Copy 4 bytes of data: reply length,
                                        // command length, command number and
                                        // a one byte parameter, all encoded as
                                        // a 32 bit unsigned integer.

    replyLen = cmndData[ 0];            // Save the first byte as reply length.
    cmndLen = cmndData[ 1];             // Save the second byte as command length.
    cmndData[ 0] = 0x5A;                // Set the first byte to the header character.

    if( cmnd == SET_FRAME_RATE)           // If the command is to Set Frame Rate...
    {
      memcpy( &cmndData[ 3], &param, 2);  // add the 2 byte Frame Rate parameter.
    }
    else if( cmnd == SET_BAUD_RATE)       // If the command is to Set Baud Rate...
    {
      memcpy( &cmndData[ 3], &param, 4);  // add the 3 byte Baud Rate parameter.
    }
    else if( cmnd == SET_I2C_ADDRESS)     // If the command to set I2C address...
    {
      memcpy( &cmndData[ 3], &param, 1);  // copy the 1 byte Address parameter.
    }

    // Create a checksum byte for the command data array.
    // chkSum variable declared in 'TFLunaI2C.h'
    chkSum = 0;
    // Add together all bytes but the last...
    for( uint8_t i = 0; i < ( replyLen - 1); i++) chkSum += reply[ i];
    // and save it as the last byte of command data.
    cmndData[ cmndLen - 1] = uint8_t( chkSum);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 2 - Send the command data array to the device
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Begin transmission to the I2C slave device
    Wire.beginTransmission( addr);
    // Queue command data array for transmission to the I2C device
    if( Wire.write( cmndData, (size_t)cmndLen) != cmndLen)
    {
        status = TFMP_I2CLENGTH;  // then set satus code...
        Wire.write( 0);           // Put a zero in the xmit buffer.
        Wire.endTransmission();   // Send and Close the I2C interface.
        return false;             // and return "false."
    }

    // Transmit the bytes and a stop message to release the I2C bus.
    if( Wire.endTransmission() != 0)  // If write error...
    {
        status = TFMP_I2CWRITE;       // then set satus code...
        return false;                 // and return "false."
    }

    // + + + + + + + + + + + + + + + + + + + + + + + + +
    // If no reply data expected, then go home. Otherwise,
    // wait for device to process the command and continue.
    if( replyLen == 0) return true;
        else delay( 500);
    // + + + + + + + + + + + + + + + + + + + + + + + + +

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 3 - Get command reply data back from the device.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    
    //  An I2C address change will take effect immediately
    //  so use the new 'param' address for the reply.
    if( cmnd == SET_I2C_ADDRESS) addr = uint8_t(param);

    // Request reply data from the device and
    // keep the I2C interface open.
    Wire.requestFrom( (int)addr, (int)replyLen, (int)1);

    memset( reply, 0, sizeof( reply));   // Clear the reply data buffer.
    for( uint8_t i = 0; i < replyLen; i++)
    {
      reply[ i] = (uint8_t)Wire.read();
    }
    
    Wire.write( 0);          // Put a zero in the xmit buffer.
    Wire.endTransmission();  // Send and Close the I2C interface.

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 4 - Perform a checksum test.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Clear the 'chkSum' variable declared in 'TFLunaI2C.h'
    chkSum = 0;
    // Add together all bytes but the last...
    for( uint8_t i = 0; i < ( replyLen - 1); i++) chkSum += reply[ i];
    // If the low order byte of the Sum does not equal the last byte...
    if( reply[ replyLen - 1] != (uint8_t)chkSum)
    {
      status = TFMP_CHECKSUM;  // then set error...
      return false;            // and return "false."
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 5 - Interpret different command responses.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if( cmnd == OBTAIN_FIRMWARE_VERSION)
    {
        version[ 0] = reply[5];  // set firmware version.
        version[ 1] = reply[4];
        version[ 2] = reply[3];
    }
    else
    {
        if( cmnd == SYSTEM_RESET ||
            cmnd == RESTORE_FACTORY_SETTINGS ||
            cmnd == SAVE_SETTINGS )
        {
            if( reply[ 3] == 1)      // If PASS/FAIL byte not zero ...
            {
                status = TFMP_FAIL;  // set status 'FAIL'...
                return false;        // and return 'false'.
            }
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 6 - Go home
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    return true;
}

// Send a command using default I2C address
bool TFLunaI2C::sendCommand( uint32_t cmnd, uint32_t param)
{
  return sendCommand( cmnd, param, TFMP_DEFAULT_ADDRESS);
}

//
// - - - - - - -  End of Send a Command  - - - - - - - - - - - -


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - -    The following is for testing purposes    - - - -

// Print status condition 'READY' or error type
// Print command reply and data-frame in hexidecimal
void TFLunaI2C::printErrorStatus()
{
    Serial.print("Status: ");
    if( status == TFMP_READY)          printf( "READY");
    else if( status == TFMP_SERIAL)    Serial.print( "SERIAL");
    else if( status == TFMP_HEADER)    Serial.print( "HEADER");
    else if( status == TFMP_CHECKSUM)  Serial.print( "CHECKSUM");
    else if( status == TFMP_TIMEOUT)   Serial.print( "TIMEOUT");
    else if( status == TFMP_PASS)      Serial.print( "PASS");
    else if( status == TFMP_FAIL)      Serial.print( "FAIL");
    else if( status == TFMP_I2CREAD)   Serial.print( "I2CREAD");
    else if( status == TFMP_I2CWRITE)  Serial.print( "I2CWRITE");
    else if( status == TFMP_I2CLENGTH) Serial.print( "I2CLENGTH");
    else Serial.print( "OTHER");
    Serial.println();
    
    Serial.print("Reply:");
    for( uint8_t i = 0; i < TFMP_REPLY_SIZE; i++)
    {
      Serial.print(" ");
      Serial.print( reply[ i], HEX);
    }
    Serial.println();
    
    Serial.print("Data:");
    for( uint8_t i = 0; i < TFMP_REPLY_SIZE; i++)
    {
      Serial.print(" ");
      Serial.print( frame[ i], HEX);
    }
    Serial.println();
}

// Prompt for Y/N response
bool TFLunaI2C::getResponse()
{
    // One second timer if serial read never occurs
    uint32_t serialTimeout = millis() + 5000;
    static char charIn;
    printf("Y/N? ");
    while( Serial.available() || ( millis() <  serialTimeout))
    {
      charIn = Serial.read();
      if( charIn == 'Y' || charIn == 'y') return true;
          else if( charIn == 'N' || charIn == 'n') return false;
    }
    return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
