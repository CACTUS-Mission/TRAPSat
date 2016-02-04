/* 
	ADS1115_sample.c - 12/9/2013. Written by David Purdie as part of the openlabtools initiative
	Initiates and reads a single sample from the ADS1115 (without error handling)
	
	edited by Zach Richard to work with the TRAPSat Custom IO board for the RockSat-x 2016 mission.
	* changed address to 0x49 (previously 0x48)
	* changed int main() to float ads1115_read()
	* changed printf("Voltage Reading %f (V) \n", (float)val*4.096/32767.0); to return (float)val*4.096/32767.0;


	USE:
	* Function accepts an integer from 0 to 3 to select which pin to read from
	* Function returns the reading from the chosen pin in mV

*/

#include "ADS1115_read.h"

float ads1115_read(int pin) {

  // only accept valid pin values
  if (pin > 3)
    return (float)(0); // not sure if this will work, I just want to return an error immediately -Zach
	
  int ADS_address = 0x49;	// Address of our device on the I2C bus
  int I2CFile;
  
  uint8_t writeBuf[3];	// Buffer to store the 3 bytes that we write to the I2C device
  uint8_t readBuf[2];		// 2 byte buffer to store the data read from the I2C device
  uint8_t ads_read_pin; // Buffer to use as a mask to select ADS input 
  
  int16_t val;				// Stores the 16 bit value of our ADC conversion
  
  I2CFile = open("/dev/i2c-1", O_RDWR);		// Open the I2C device
  
  ioctl(I2CFile, I2C_SLAVE, ADS_address);   // Specify the address of the I2C Slave to communicate with
	  
  // These three bytes are written to the ADS1115 to set the config register and start a conversion 
  writeBuf[0] = 1;			// This sets the pointer register so that the following two bytes write to the config register
  writeBuf[1] = 0xC3;   	// This sets the 8 MSBs of the config register (bits 15-8) to 11000011 -- C can be changed to C-F to select the channel to read. Default as C for AIN0
  writeBuf[2] = 0x03;  		// This sets the 8 LSBs of the config register (bits 7-0) to 00000011
  
  // Set the pin to be read from based on input parameter
  switch (pin)
  {
    case 0 : ads_read_pin = 0x40; break;
    case 1 : ads_read_pin = 0x50; break;
    case 2 : ads_read_pin = 0x60; break;
    case 3 : ads_read_pin = 0x70; break;
    default: ads_read_pin = 0x40; break;
  }
  
  writeBuf[1] |= ads_read_pin; // mask other set flags, only change the read pin

  // Initialize the buffer used to read data from the ADS1115 to 0
  readBuf[0]= 0;		
  readBuf[1]= 0;
	  
  // Write writeBuf to the ADS1115, the 3 specifies the number of bytes we are writing,
  // this begins a single conversion
  write(I2CFile, writeBuf, 3);	

  // Wait for the conversion to complete, this requires bit 15 to change from 0->1
  while ((readBuf[0] & 0x80) == 0)	// readBuf[0] contains 8 MSBs of config register, AND with 10000000 to select bit 15
  {
	  read(I2CFile, readBuf, 2);	// Read the config register into readBuf
  }

  writeBuf[0] = 0;					// Set pointer register to 0 to read from the conversion register
  write(I2CFile, writeBuf, 1);
  
  read(I2CFile, readBuf, 2);		// Read the contents of the conversion register into readBuf

  val = readBuf[0] << 8 | readBuf[1];	// Combine the two bytes of readBuf into a single 16 bit result 
		
  close(I2CFile);
  
  return (float)val*4.096/32767.0; // return the Voltage read in mV

}


