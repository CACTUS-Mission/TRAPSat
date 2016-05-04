/**
 * ADS1115_sample.c - 12/9/2013. Written by David Purdie as part of the
 * openlabtools initiative
 *
 * Initiates and reads a single sample from the ADS1115 (without error
 * handling)
 *
 * Edited by Zach Richard to work with the TRAPSat Custom IO board for the
 * RockSat-x 2016 mission.
 *  - changed address to 0x49 (previously 0x48)
 *  - changed int main() to float ads1115_read()
 *	- changed printf("Voltage Reading %f (V) \n",
 *                    (float)val*4.096/32767.0);
 *    to return (float)val*4.096/32767.0;
 */

#include "ADS1115_read.h"

/**
 * Reads the voltage from the specified pin. (0-3 are valid)
 * @param[in] pin The pin to read from
 * @returns The mV of the pin as a float. Returns absolute zero if invalid
 *          pin provided.
 */
float ads1115_read(int pin) {
  // only accept valid pin values
  if (pin > 3) {
    // Return 0.0 to indicate invalid pin
    return (float)(0);
  } else {
    /** Address of our device on the I2C bus */
    int ADS_address = 0x49;
    /** The file handle for the I2C bus */
    int I2CFile;

    /** Buffer to store the 3 bytes that we write to the I2C device */
    uint8_t writeBuf[3];
    /** 2 byte buffer to store the data read from the I2C device */
    uint8_t readBuf[2];
    /** Buffer to use as a mask to select ADS input */
    uint8_t ads_read_pin;

    /** Stores the 16 bit value of our ADC conversion */
    int16_t val;

    // Open the I2C device
    I2CFile = open("/dev/i2c-1", O_RDWR);

    // Specify the address of the I2C Slave to communicate with
    ioctl(I2CFile, I2C_SLAVE, ADS_address);

    /* These three bytes are written to the ADS1115 to set the config register
       and start a conversion. */

    /* This sets the pointer register so that the following two
       bytes write to the config register. */
    writeBuf[0] = 1;

    /* This sets the 8 MSBs of the config register (bits 15-8)
     * to 11000011
     * The first byte can be changed to C-F to select the channel to read.
     * Default as C for AIN0. */
    writeBuf[1] = 0xC3;

    // This sets the 8 LSBs of the config register (bits 7-0) to 00000011
    writeBuf[2] = 0x03;

    // Set the pin to be read from based on input parameter
    switch (pin) {
    case 0:
      ads_read_pin = 0x40;
      break;
    case 1:
      ads_read_pin = 0x50;
      break;
    case 2:
      ads_read_pin = 0x60;
      break;
    case 3:
      ads_read_pin = 0x70;
      break;
    default:
      ads_read_pin = 0x40;
      break;
    }

    // mask other set flags, only change the read pin
    writeBuf[1] |= ads_read_pin;

    // Initialize the buffer used to read data from the ADS1115 to 0
    readBuf[0] = 0;
    readBuf[1] = 0;

    /* Write writeBuf to the ADS1115, the 3 specifies the number of bytes we are
     * writing, this begins a single conversion */
    write(I2CFile, writeBuf, 3);

    /* Wait for the conversion to complete.
     * When complete, bit 15 changes from 0->1.
     * readBuf[0] contains 8 MSBs of config.
     * Bitwise AND of readBuf and 10000000
     * to watch for bit 15. */
    while ((readBuf[0] & 0x80) == 0) {
      // While bit 15 is 0, read the config register into readBuf
      read(I2CFile, readBuf, 2);
    }

    // Set pointer register to 0 to read from the conversion register
    writeBuf[0] = 0;
    // Write new pointer register to I2C
    write(I2CFile, writeBuf, 1);

    // Read the contents of the conversion register into readBuf
    read(I2CFile, readBuf, 2);

    // Combine the two bytes of readBuf into a single 16 bit result
    val = readBuf[0] << 8 | readBuf[1];

    // Close I2C device file handle
    close(I2CFile);

    // TODO: clarify if this is mV or V.. comments have mixed opinions.
    // return the Voltage read in Volts
    return (float)val * 4096 / 32767.0;
  }
}