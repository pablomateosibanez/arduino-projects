/* File Name: TFLunaI2C.h
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
 *     dist - distance to target in centimeters: 10cm - 1200cm,
 *     flux - strength, voltage or quality of returned signal
 *            in arbitrary units: 0 - 65535,
 *     temp - chip temperature: -25°C to 125°C,
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

#ifndef TFLunaI2C_H       // Guard to compile only once
#define TFLunaI2C_H

#include <Arduino.h>    // Always include this. It's important.

// Buffer sizes
#define TFMP_FRAME_SIZE         9   // Size of data frame = 9 bytes
#define TFMP_REPLY_SIZE         8   // Longest command reply = 8 bytes
#define TFMP_COMMAND_MAX        8   // Longest command = 8 bytes

// Timeout Limits for various functions
#define TFMP_MAX_READS           20   // readData() sets SERIAL error
#define MAX_BYTES_BEFORE_HEADER  20   // getData() sets HEADER error
#define MAX_ATTEMPTS_TO_MEASURE  20

#define TFMP_DEFAULT_ADDRESS   0x10   // default I2C slave address
                                      // as hexidecimal integer
// System Error Status Condition
#define TFMP_READY           0  // no error
#define TFMP_SERIAL          1  // serial timeout
#define TFMP_HEADER          2  // no header found
#define TFMP_CHECKSUM        3  // checksum doesn't match
#define TFMP_TIMEOUT         4  // I2C timeout
#define TFMP_PASS            5  // reply from some system commands
#define TFMP_FAIL            6  //           "
#define TFMP_I2CREAD         7
#define TFMP_I2CWRITE        8
#define TFMP_I2CLENGTH       9
#define TFMP_MEASURE        10

// The library 'sendCommand( cmnd, param)' function
// defines a 'cmnd' in the the following format:
// 0x     00       00       00       00
//     one byte  command  command   reply
//     payload   number   length    length
#define    I2C_FORMAT_CM              0x01000500   // returns a 9 byte data frame
#define    OBTAIN_FIRMWARE_VERSION    0x00010407   // returns 3 byte firmware version

#define    SYSTEM_RESET               0x00020405   // returns a 1 byte pass/fail (0/1)
#define    RESTORE_FACTORY_SETTINGS   0x00100405   //           "
#define    SAVE_SETTINGS              0x00110405   //           "

#define    SET_FRAME_RATE             0x00030606   // returns an echo of the command
#define    SET_BAUD_RATE              0x00060808   //           "
#define    ENABLE_OUTPUT              0x00070505   //           "
#define    DISABLE_OUTPUT             0x01070505   //           "
#define    SET_I2C_ADDRESS            0x100B0505   //           "

#define    SET_UART_MODE              0x000A0500   // returns no reply data
#define    SET_I2C_MODE               0x010A0500   //           "

// Command Parameters
#define    FRAME_0            0x0000     // internal measurement rate
#define    FRAME_1            0x0001     // expressed as hexidecimal
#define    FRAME_2            0x0002
#define    FRAME_5            0x0003
#define    FRAME_10           0x000A
#define    FRAME_20           0x0014
#define    FRAME_25           0x0019
#define    FRAME_50           0x0032
#define    FRAME_100          0x0064
#define    FRAME_125          0x007D
#define    FRAME_200          0x00C8
#define    FRAME_250          0x00FA
#define    FRAME_500          0x01F4
#define    FRAME_1000         0x03E8

//#define    BAUD_9600          0x002580   // UART serial baud rate
//#define    BAUD_14400         0x003840   // expressed as hexidecimal
//#define    BAUD_19200         0x004B00   // **  not used in I2C  **
//#define    BAUD_56000         0x00DAC0
//#define    BAUD_115200        0x01C200
//#define    BAUD_460800        0x070800
//#define    BAUD_921600        0x0E1000


// Object Class Definitions
class TFLunaI2C
{
  public:
    TFLunaI2C();
    ~TFLunaI2C();

    uint8_t version[ 3];   // three digit firmware version
    uint8_t format;        // distance data format: CENTI or MILLI
    char scale;            // temperature scale: FAREN or CELSI
    uint8_t status;        // system error status: READY = 0

    // Get device data-frame and pass back three values
    bool getData( uint16_t &dist, uint16_t &flux, uint16_t &temp, uint8_t addr);
    bool getData( uint16_t &dist, uint16_t &flux, uint16_t &temp);
    bool getData( uint16_t &dist);

    // Build and send a command, and check response
    bool sendCommand( uint32_t cmnd, uint32_t param, uint8_t addr);
    bool sendCommand( uint32_t cmnd, uint32_t param);

    // for testing purposes, print frame or reply data and status
    void printErrorStatus();
    bool getResponse();

  private:
    uint8_t frame[ TFMP_FRAME_SIZE + 1];
    uint8_t reply[ TFMP_REPLY_SIZE + 1];

    uint16_t chkSum;     // to calculate the check sum byte.
    uint8_t replyLen;    // reply data length

};

#endif
