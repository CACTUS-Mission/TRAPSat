/*******************************************************************************
** File: ads1115_device.c
**
** Purpose:
**   This is a source file for the ADS1115 application
**   
**   int ADS1115_ReadADCChannels(void) "ADS1115 REad ADC Channels Loop"
**     -  
** This function should be called from the ADS1115_ADC_ChildTask
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
** int ADS1115_ReadADCChannels(void)
**
** Read from all four channels of the ADS1115
** (Analog to Digital Converter (16-bit buffer per channel))
** Assumed to be mounted at /dev/i2c-1
**
** Returns:
**    zero on success
**    negative value on error
*/
int ADS1115_ReadADCChannels(void)
{
    int i2c_fd;
    char *dev_i2c_f = "/dev/i2c-1";
    int io_res;
    uint8 i2c_cfg_data[3];
    uint8 i2c_data[2];
    uint16 i2c_data_word;
    int ads_ch_mask = 0;
    int adc_ch_sel = 0;
    
    /*
    **
    */
    if ((i2c_fd = open(dev_i2c_f, O_RDWR)) < 0)
    {
        OS_printf( "Failed opening \'%s\' with return value %d\n", dev_i2c_f, i2c_fd);
        return -1;
    }
    else
    {
        OS_printf("Debug: open(%s) = %d\n", dev_i2c_f, i2c_fd);
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
    ** For each ADC channel
    ** Read the voltage -> store the value in HK
    */
    for( adc_ch_sel = 0; adc_ch_sel < 4; adc_ch_sel++)
    {
        /*
        ** Reset ADC data/config registers
        */
        i2c_cfg_data[0] = 0x01; /* Address config reg */
        i2c_cfg_data[1] = 0xC3; /* MSB of config reg */
        i2c_cfg_data[2] = 0x03; /* LSB of config reg */

        /*
        ** Clear Data Buffer of all previous data
        */
        i2c_data[0] = 0x00;
        i2c_data[1] = 0x00;
        
        /*
        ** Set the ADC Channel Selector mask
        */
        switch (adc_ch_sel)
        {
          case 0 : ads_ch_mask = 0x40; break;
          case 1 : ads_ch_mask = 0x50; break;
          case 2 : ads_ch_mask = 0x60; break;
          case 3 : ads_ch_mask = 0x70; break;
          default: ads_ch_mask = 0x40; break;
        }
        
        /*
        ** Adjust Config Flags with channel mask
        */
        i2c_cfg_data[1] |= ads_ch_mask; 
          
        /*
        ** Configure I2C device
        */
         if ((io_res = write(i2c_fd, i2c_cfg_data, 3)) < 0)
        {
            OS_printf( "I2C Configuration Failure: Returned %d\n", io_res);
            //return -1;
        }
        else if (io_res != 3)
        {
            OS_printf( "I2C Configuration Error: expected to write 3 bytes, %d bytes written\n", io_res);
            //return -1;
        }

        /*
        ** Wait for Operational Status flag to be set 
        ** (bit 15 of config register) 
        ** (0=Busy/1=Not Busy)
        */
        while ( !(i2c_data[0] & 0x80) )
        {
            if ((io_res = read(i2c_fd, i2c_data, 2)) < 0)
            {
                OS_printf( "I2C Read Failure: Returned %d\n", io_res);
                //return(-1);
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
        
        OS_printf("Channel %d Voltage: %f V\n", adc_ch_sel, (float) i2c_data_word*4.096/32767.0);

        /*
        ** double check sizeof(ADS1115_ChannelData.adc_ch_1)
        ** OS_printf("sizeof(ADS1115_ChannelData.adc_ch_1) = %d\n", sizeof(ADS1115_ChannelData.adc_ch_1));
        */
        memcpy((&ADS1115_ChannelData.adc_ch_1 + (sizeof(ADS1115_ChannelData.adc_ch_1)*adc_ch_sel)), 
                &i2c_data_word, 
                sizeof(ADS1115_ChannelData.adc_ch_1));        
    } /* Channel Select Loop End Here */

    close(i2c_fd);

    return(0);
}

/*
** int ADS1115_StoreADCChannels(void)
**
** Read the ADC channel data struct, store to file
** Returns
**    0 on success.
**    negative on error
*/
int ADS1115_StoreADCChannels(void)
{
    /*
    ** Return Value for OSAL calls
    */
    int32 os_ret_val = 0;

    /*
    ** File Descriptor buffer for OS_open
    */
    int32 os_fd = 0;

    /*
    ** virtual_path[] holds the cFS-relative location
    ** exe/ram/temps/ must be created before running
    */
    char virtual_path[] = {"/ram/temps/"};

    /*
    ** local_path[] will hold the platform-absolute location
    */
    char local_path[ OS_MAX_PATH_LEN ];

    /*
    ** buffer for the filename. This will need to be filled
    ** with num-restarts_time.csv
    ** double check how small we can make this buffer
    */
    char data_filename[32];

    /*
    ** file mode flags
    */
    mode_t file_mode = ((S_IRUSR | S_IWUSR) |
                        (S_IRGRP | S_IWGRP) |
                        (S_IROTH | S_IWOTH));

    /*
    ** full_path[] will hold the path and filename together
    */
    char full_path[ OS_MAX_LOCAL_PATH_LEN ];

    /*
    ** Concatonate filename
    */
    sprintf(data_filename, "%s.csv", "temps");

    /*
    ** Translate the relative path to an absoulte path
    */
    if ( (os_ret_val = OS_TranslatePath(virtual_path, (char *)local_path)) != OS_FS_SUCCESS )
    {
        OS_printf("OS_TranslatePath Status: %d \n", os_ret_val);
        return -1;
    }

    /*
    ** concatonate path and filename
    */
    sprintf(full_path, "%s%s", local_path, data_filename);

    
    OS_printf("Local Path: %s\n", local_path);    
    OS_printf("Full Path: %s\n", full_path);

    /* 
    ** Open Data File
    */
    if ((os_fd = OS_open(full_path, OS_READ_WRITE, file_mode)) < 0)
    {
        OS_printf("ADC file could not be opened.");
        return -1;
    }

    /*
    ** Write adc channel data to data file
    ** from start of channel data (&ADS1115_ChannelData.adc_ch_1)
    ** continuing 8 bytes (2 bytes per channel, 4 channels)
    */
    if ( (os_ret_val = OS_write(os_fd, &ADS1115_ChannelData.adc_ch_1, 8)) < 0 )
    {
        OS_printf("OS_TranslatePath Status: %d \n", os_ret_val);
        return -1;
    }


    OS_close(os_fd);

    return(0);
}