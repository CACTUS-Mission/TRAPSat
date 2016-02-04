/*
** This is going to be a long night.
*/

#include <stdio.h>
//#include <stdlib.h>
#include <stdint.h>
//#include <string.h>
//#include <limits.h>
//#include <unistd.h> /* Unix standard function definitions */
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitons */
//#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

//#include "ads1115_device.h"


int rpicm_i2c_detect_bus()
{
	int fd;
	
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

	/*
	** Could not find I2C bus
	*/
	close(fd);
	return -1;
}

int rpicm_i2c_open_bus(int i2c_bus)
{
	if (i2c_bus < 0 || i2c_bus > 2)
	{
		fprintf(stderr, "rpicm_i2c_open_bus(int) recieved unexpected arg (%d)\n", i2c_bus);
		return -1;
	}
	
	//printf("debug: inside int rpicm_i2c_open_bus(int) - passed arg error\n");

	int fd;
	char filename[12];
	char i2c_bus_id = i2c_bus + '0'; /* convert to ascii*/
	
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
	
	/*
	** Return successfully opened file descriptor
	*/
	return fd;
}

#define ADS1115_ADDR  0b01001001                                    /* (0x49) */
#define ADS1115_CONV_ADDR 0b00000000                         /* (0x00) */
#define ADS1115_CFG_ADDR 0b00000001                            /* (0x01) */

#define ADS1115_POST_DATA_CMD        0b10010001         /* (0x91) */
#define ADS1115_LISTEN_CMD                0b10010000         /* (0x90) */

#define ADS1115_CFG_DEFAULT_MSB    0x85
#define ADS1115_CFG_DEFAULT_LSB    0x83


int main(int argc, char *argv[])
{
    int bus_id;
    int i2c_fd;
    
    if ((bus_id = rpicm_i2c_detect_bus()) < 0)
    {
        fprintf(stderr, "error: rpicm_i2c_detect_bus() = %d\n", bus_id);
        exit(-1);
    }
    else
    {
	    printf("rpicm_i2c_detect_bus() = %d\n", bus_id);
	}
	
	if ((i2c_fd = rpicm_i2c_open_bus(bus_id)) < 0)
	{
	    fprintf(stderr, "error: rpicm_i2c_open_bus(%d) = %d\n", bus_id, i2c_fd);
	    exit(-1);
	}
	else
	{
    	printf("rpicm_i2c_open_bus(%d) = %d\n", bus_id, i2c_fd); 
	}
	
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
        exit(-1);
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
    
    uint16_t val = i2c_data[0] << 8 | i2c_data[1];	// Combine the two bytes of readBuf into a single 16 bit result 
		
    close(i2c_fd);
  
    printf("Voltage: %f V\n", val*4.096/32767.0);
    
    
	return 0;
}



/*
** End of file.
*/
