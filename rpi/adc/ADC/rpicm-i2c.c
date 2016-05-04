/*
* rpicm-i2c.c - I2C functions for the Raspberry Pi Compute Module.
*/

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitons */
#include <linux/i2c-dev.h>

//#include <stdlib.h>
//#include <string.h>
//#include <limits.h>
//#include <unistd.h> /* Unix standard function definitions */
//#include <sys/ioctl.h>

//#include "ads1115_device.h"

/**
* Detects where the I2C bus is attached.
*/
int rpicm_i2c_detect_bus()
{
	int fd;
	
	// Check on /dev/i2c-0 through i2c-2.
	if ((fd = open("/dev/i2c-0", O_RDWR)) != -1)
	{
		close(fd);
		return 0;
	}
	else if ((fd = open("/dev/i2c-1", O_RDWR))  != -1)
	{
		close(fd);
		return 1;
	}
	else if ((fd = open("/dev/i2c-2", O_RDWR))  != -1)
	{
		close(fd);
		return 2;
	}

	// Could not find I2C bus
	close(fd);
	return -1;
}

/**
* Opens the specified I2C bus.
* @param[in] i2c_bus The bus number to open
* @returns The opened file descriptor for the I2C bus
*/
int rpicm_i2c_open_bus(int i2c_bus)
{
	// Make sure the specified I2C bus is actually one that exists
	if (i2c_bus < 0 || i2c_bus > 2)
	{
		fprintf(stderr, "rpicm_i2c_open_bus(int) recieved unexpected arg (%d)\n", i2c_bus);
		return -1;
	}
	
	//printf("debug: inside int rpicm_i2c_open_bus(int) - passed arg error\n");

	int fd;
	char filename[12];
	/** The I2C bus ID as an ASCII character */
	char i2c_bus_id = (char) i2c_bus;
	
	//printf("debug: inside int rpicm_i2c_open_bus(int) - passed convert\n");

	if ((snprintf(filename, sizeof(filename), "/dev/i2c-%c", i2c_bus_id)) < 0)
	{
		fprintf(stderr, "rpicm_i2c_open_bus(int) snprintf error\n");
		return -1;
	}
	
	//printf("debug: inside int rpicm_i2c_open_bus(int) - passed concat\n");
	
	if ((fd = open(filename, O_RDWR)) < 0)
	{
		fprintf(stderr, "rpicm_i2c_open_bus(int) failed to open %s\n", filename);
		return -1;
	}
	
	//printf("debug: inside int rpicm_i2c_open_bus(int) - passed open file\n");
	
	// Return successfully opened file descriptor
	return fd;
}

/** Hardware address for the ADS1115 analog-digital converter */
const uint8_t ADS1115_ADDR = 0x49;
/** TODO: document this constant */
const uint8_t ADS1115_CONV_ADDR = 0x00;
/** TODO: document this constant */
const uint8_t ADS1115_CFG_ADDR = 0x01;

/** TODO: document this constant */
const uint8_t ADS1115_POST_DATA_CMD = 0x91;
/** TODO: document this constant */
const uint8_t ADS1115_LISTEN_CMD = 0x90;

/** Default most significant bit */
const uint8_t ADS1115_CFG_DEFAULT_MSB = 0x85;
/** Default least significant bit */
const uint8_t ADS1115_CFG_DEFAULT_LSB = 0x83;


int main(int argc, char *argv[])
{
	/** The I2C bus ID */
	int bus_id;
	/** The I2C bus file descriptor */
	int i2c_fd;
	
	if ((bus_id = rpicm_i2c_detect_bus()) < 0)
	{
		// Detecting bus failed, exit -1
		fprintf(stderr, "error: rpicm_i2c_detect_bus() = %d\n", bus_id);
		return -1;
	}
	else
	{
		printf("rpicm_i2c_detect_bus() = %d\n", bus_id);
		if ((i2c_fd = rpicm_i2c_open_bus(bus_id)) < 0)
		{
			// Opening bus failed, exit -1
			fprintf(stderr, "error: rpicm_i2c_open_bus(%d) = %d\n", bus_id, i2c_fd);
			return -1;
		}
		else
		{
			printf("rpicm_i2c_open_bus(%d) = %d\n", bus_id, i2c_fd);
			/* Testing */

			int io_res;
			uint8_t i2c_cfg_data[3] = {0x01, 0xC3, 0x03};
			uint8_t i2c_data[2] = {0x00, 0x00};

			/*
			** Address the I2C device
			*/
			if ((io_res = ioctl(i2c_fd, I2C_SLAVE, ADS1115_ADDR)) < 0)
			{
				fprintf(stderr, "IO Control Failed: Returned %c\n", strerror(io_res));
				return -1;
			}

			/*
			** Configure I2C device
			*/
			if ((io_res =write(i2c_fd, i2c_cfg_data, 3)) < 0)
			{
				fprintf(stderr, "I2C Configuration Failure: Returned %c\n", strerror(io_res));
			}
			else if (io_res != 3)
			{
				fprintf(stderr, "I2C Configuration Error: expected to write 3 bytes, %d bytes written\n", strerror(io_res));
			}

			/*
			** Wait for Operational Status flag (bit 15 of config register) to be set - (0=Busy/1=Not Busy)
			*/
			while ( !(i2c_data[0] & 0x80) )
			{
				if ((io_res = read(i2c_fd, i2c_data, 2)) < 0)
				{
					fprintf(stderr, "I2C Read Failure: Returned %c\n", strerror(io_res));
				}
			}
			
			/*
			** Set Pointer Register to 0 (To read from conversion register)
			*/
			i2c_cfg_data[0] = 0x00; 
			if ((io_res = write(i2c_fd, i2c_cfg_data, 1)) < 0)
			{
				fprintf(stderr, "I2C Configuration Failure: Returned %c\n", strerror(io_res));
			}
			else if (io_res != 1)
			{
				fprintf(stderr, "I2C Configuration Error: expected to write 1 bytes, %d bytes written\n", strerror(io_res));
			}
			
			if ((io_res = read(i2c_fd, i2c_data, 2)) < 0)
			{
				fprintf(stderr, "I2C Read Failure: Returned %c\n", strerror(io_res));
			}
			
			// Combine the two bytes of readBuf into a single 16 bit result 
			uint16_t val = i2c_data[0] << 8 | i2c_data[1];
			
			close(i2c_fd);

			printf("Voltage: %f V\n", val*4.096/32767.0);
			
			
		}
	}
	return 0;
}