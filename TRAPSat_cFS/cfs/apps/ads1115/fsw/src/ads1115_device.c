/*******************************************************************************
** File: ads1115_device.c
**
** Purpose:
**   This is a source file for the ADS1115 application
**
*******************************************************************************/
/*
** ADS1115 Master Header
*/
#include "ads1115.h"
#include "ads1115_perfids.h"
#include "ads1115_msgids.h"
#include "ads1115_msg.h"
#include "ads1115_events.h"
#include "ads1115_version.h"
#include "ads1115_child.h"


/*
** External References
*/
extern ADS1115_Ch_Data_t ADS1115_ChannelData;


/*
** ADS1115 Read ADC Channels Loop
*/
int ADS1115_ReadADCChannels(void)
{
    int i2c_fd;
	char *dev_i2c_f = "/dev/i2c-1";
    int io_res;
    uint8_t i2c_cfg_data[3];
    uint8_t i2c_data[2];
    uint16_t i2c_data_word;
	
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
    	OS_printf("Debug: open(%s) = %d\n", dev_i2c_f, i2c_fd);
    	/*printf("open(%d) = %d\n", bus_id, i2c_fd); */
	}
	
	/*
	** Address the I2C device
	*/
    if ((io_res = ioctl(i2c_fd, I2C_SLAVE, ADS1115_ADDR)) < 0)
    {
        OS_printf("IO Control Failed: Returned %d\n", io_res);
        return -1;
    }

    /*
    ** infinite read loop
    ** w/ delay 
    */
    for ( ; ; )
    {
	    i2c_cfg_data[0] = 0x01;
        i2c_cfg_data[1] = 0xC3;
        i2c_cfg_data[2] = 0x03;
	    i2c_data[0] = 0x00;
        i2c_data[1] = 0x00;

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
	    
	    i2c_data_word = i2c_data[0] << 8 | i2c_data[1];
	  
	    OS_printf("Voltage: %f V\n", (float) i2c_data_word*4.096/32767.0);

        memcpy(&ADS1115_ChannelData.adc_ch_1, &i2c_data_word, size_of(adc_ch_1));

	    OS_TaskDelay(2000);
	}

    close(i2c_fd);

   return (0);
}
