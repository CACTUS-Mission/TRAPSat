/*******************************************************************************
** File: ads1115_device.c
**
** Purpose:
**   This is a source file for the ADS1115 application
**
*******************************************************************************/

#include <stdio.h>   /* Standard input/output definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "cfe.h"
#include "ads1115.h"
#include "ads1115_msg.h"
#include "ads1115_events.h"

/*
** References
*/

/*
** External Variables
*/
extern ADS1115_Ch_Data_t ADS1115_ChannelData;

/*
** Defines moved to other file "ads1115.c" 
*/
//extern uint8 NAV_BMP085_mode_option;
//extern uint16 NAV_gyro_range_mode;

/*
**
*/
int ADS1115_ReadADCChannels(void)
{
    int i2c_fd;
	char *dev_i2c_f = "/dev/i2c-1";
    int io_res;
    uint8_t i2c_cfg_data[3];
    uint8_t i2c_data[2];
	
	if ((i2c_fd = open(dev_i2c_f, O_RDWR)) < 0)
	{
	    OS_printf( "Failed opening \'%s\' with return value %d\n", dev_i2c_f, i2c_fd);
	    return -1;
	}
	else
	{
    	/*
    	** Debug
    	*/
    	/*printf("open(%d) = %d\n", bus_id, i2c_fd); */
	}
	
	/*
	** Address the I2C device
	*/
    if ((io_res = ioctl(i2c_fd, I2C_SLAVE, ADS1115_ADDR)) < 0)
    {
        OS_printf( "IO Control Failed: Returned %d\n", io_res);
        exit(-1);
    }
    

    i2c_cfg_data[3] = {0x01, 0xC3, 0x03};
    i2c_data[2] = {0x00, 0x00};

    /*
    ** Configure I2C device
    */
     if ((io_res = write(i2c_fd, i2c_cfg_data, 3)) < 0)
    {
        OS_printf( "I2C Configuration Failure: Returned %d\n", io_res);
    }
    else if (io_res != 3)
    {
        OS_printf( "I2C Configuration Error: expected to write 3 bytes, %d bytes written\n", io_res);
    }

    /*
    ** Wait for Operational Status flag (bit 15 of config register) to be set - (0=Busy/1=Not Busy)
    */
    while ( !(i2c_data[0] & 0x80) )
    {
        if ((io_res = read(i2c_fd, i2c_data, 2)) < 0)
        {
            OS_printf( "I2C Read Failure: Returned %d\n", io_res);
        }
    }
    
    /*
    ** Set Pointer Register to 0 (To read from conversion register)
    */
    i2c_cfg_data[0] = 0x00; 
    if ((io_res = write(i2c_fd, i2c_cfg_data, 1)) < 0)
    {
        OS_printf( "I2C Configuration Failure: Returned %d\n", io_res);
    }
    else if (io_res != 1)
    {
        OS_printf( "I2C Configuration Error: expected to write 1 bytes, %d bytes written\n", io_res);
    }
    
    if ((io_res = read(i2c_fd, i2c_data, 2)) < 0)
    {
        OS_printf( "I2C Read Failure: Returned %d\n", io_res);
    }
    
    uint16_t val = i2c_data[0] << 8 | i2c_data[1];	// Combine the two bytes of readBuf into a single 16 bit result 
		
    close(i2c_fd);
  
    OS_printf("Voltage: %f V\n", (float) val*4.096/32767.0);
    
    /*
    ** infinite read loop
    ** w/ delay 
    */
    /*while(1)
    {
        OS_TaskDelay(2000);
    }*/
   return 0;
}
