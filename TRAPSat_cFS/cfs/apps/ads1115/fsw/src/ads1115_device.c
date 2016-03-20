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
** Global Variables
*/
uint16 ads1115_adc_read_count;

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
        OS_printf( "ADS1115: Failed opening \'%s\' with return value %d\n", dev_i2c_f, i2c_fd);
        return -1;
    }
    else
    {
        OS_printf("ADS1115: open(%s) = %d\n", dev_i2c_f, i2c_fd);
    }
    
    /*
    ** Address the I2C device
    */
    if ((io_res = ioctl(i2c_fd, I2C_SLAVE, ADS1115_ADDR)) < 0)
    {
        OS_printf("ADS1115: IO Control Failed: Returned %d\n", io_res);
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
            OS_printf( "ADS1115: I2C Configuration Failure: Returned %d\n", io_res);
            return -1;
        }
        else if (io_res != 3)
        {
            OS_printf( "ADS1115: I2C Configuration Error: expected to write 3 bytes, %d bytes written\n", io_res);
            return -1;
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
                OS_printf( "ADS1115: I2C Read Failure: Returned [%d]\n", io_res);
                //return(-1);
            }
        }
        
        /*
        ** Set Pointer Register to 0 (To read from conversion register)
        */
        i2c_cfg_data[0] = 0x00;
        if ((io_res = write(i2c_fd, i2c_cfg_data, 1)) < 0)
        {
            OS_printf( "ADS1115: I2C Configuration Failure: Returned %d\n", io_res);
        }
        else if (io_res != 1)
        {
            OS_printf( "ADS1115: I2C Configuration Error: expected to write 1 bytes, %d bytes written\n", io_res);
        }
        
        if ((io_res = read(i2c_fd, i2c_data, 2)) < 0)
        {
            OS_printf( "ADS1115: I2C Read Failure: Returned %d\n", io_res);
        }
        
        /*
        **print values here
        */
        OS_printf("After Read:\n");
        OS_printf("\'i2c_data[index]\': &i2c_data[index], i2c_data[index]\n");
        OS_printf("i2c_data[%d]: *[%d] = [%#X]\n", 0, &i2c_data[0], i2c_data[0]);
        OS_printf("i2c_data[%d]: *[%d] = [%#X]\n", 1, &i2c_data[1], i2c_data[1]);
        OS_printf("..\n");
        OS_printf("Before pack:\n");
        OS_printf("i2c_data_word: [%d], [%#X]\n", &i2c_data_word, i2c_data_word);


        i2c_data_word = i2c_data[0] << 8 | i2c_data[1];

        OS_printf("After pack:\n");
        OS_printf("i2c_data_word: *[%d] = [%#X]\n", &i2c_data_word, i2c_data_word);

        
        OS_printf("ADS1115: Channel %d Voltage: %f V\n", adc_ch_sel, (float) i2c_data_word*4.096/32767.0);

        /*
        ** double check ADS1115_ADC_CH_BUF_SIZE
        OS_printf("sizeof(ADS1115_ChannelData.adc_ch_1) = %d\n", sizeof(ADS1115_ChannelData.adc_ch_1));
        */


        OS_printf("Before copy:\n");
        OS_printf("ADS1115_ChannelData.adc_ch_0[0]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_0[0], ADS1115_ChannelData.adc_ch_0[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_0[1]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_0[1], ADS1115_ChannelData.adc_ch_0[1]);

        OS_printf("ADS1115_ChannelData.adc_ch_1[0]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_1[0], ADS1115_ChannelData.adc_ch_1[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_1[1]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_1[1], ADS1115_ChannelData.adc_ch_1[1]);


        OS_printf("ADS1115_ChannelData.adc_ch_2[0]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_2[0], ADS1115_ChannelData.adc_ch_2[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_2[1]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_2[1], ADS1115_ChannelData.adc_ch_2[1]);

        OS_printf("ADS1115_ChannelData.adc_ch_3[0]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_3[0], ADS1115_ChannelData.adc_ch_3[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_3[1]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_3[1], ADS1115_ChannelData.adc_ch_3[1]);

        /*
        ** Copy Data to ChannelData struct
        **
        */
        /*
        memcpy((&ADS1115_ChannelData.adc_ch_1 + (ADS1115_ADC_CH_BUF_SIZE*adc_ch_sel)), 
                &i2c_data_word, 
                ADS1115_ADC_CH_BUF_SIZE);
        */
        /*
        *((uint8 *) ADS1115_ChannelData.adc_ch_0 + (void) (ADS1115_ADC_CH_BUF_SIZE*adc_ch_sel)) = i2c_data[0];
        *((uint8 *) ADS1115_ChannelData.adc_ch_0 + (void) (ADS1115_ADC_CH_BUF_SIZE*adc_ch_sel) + 1) = i2c_data[1];
        */

        memcpy((&ADS1115_ChannelData.adc_ch_0 + (ADS1115_ADC_CH_BUF_SIZE*adc_ch_sel)), 
                &i2c_data, 
                ADS1115_ADC_CH_BUF_SIZE);
        
        OS_printf("After copy:\n");
        OS_printf("ADS1115_ChannelData.adc_ch_0[0]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_0[0], ADS1115_ChannelData.adc_ch_0[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_0[1]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_0[1], ADS1115_ChannelData.adc_ch_0[1]);

        OS_printf("ADS1115_ChannelData.adc_ch_1[0]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_1[0], ADS1115_ChannelData.adc_ch_1[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_1[1]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_1[1], ADS1115_ChannelData.adc_ch_1[1]);


        OS_printf("ADS1115_ChannelData.adc_ch_2[0]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_2[0], ADS1115_ChannelData.adc_ch_2[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_2[1]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_2[1], ADS1115_ChannelData.adc_ch_2[1]);

        OS_printf("ADS1115_ChannelData.adc_ch_3[0]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_3[0], ADS1115_ChannelData.adc_ch_3[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_3[1]: *[%d] = [%#X]\n", &ADS1115_ChannelData.adc_ch_3[1], ADS1115_ChannelData.adc_ch_3[1]);


        /*
        float voltage = (float)(uint16)(uint16 *)(ADS1115_ChannelData.adc_ch_1 + (ADS1115_ADC_CH_BUF_SIZE*adc_ch_sel));
        voltage *= (4.096/32767.0);

        OS_printf("ADS1115: Struct Data => Channel %d Voltage: %f V\n", adc_ch_sel, voltage); 
        */


    } /* Channel Select Loop End Here */

    /*
    ** Increment ADC Read sample counter after 
    */
    ads1115_adc_read_count++;

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
    char virtual_path[OS_MAX_PATH_LEN];
    if((os_ret_val = sprintf(virtual_path, "%s", "/ram/temps/") < 0))
    {
        OS_printf("ADS1115: sprintf returned [%d] expected non-negative\n", os_ret_val);
        OS_printf("ADS1115: target virtual_path: \'%s\'\n", virtual_path);
    }

    /*
    ** Longest filename: 000_tempset_00000.csv (21 chars)
    **  - data_filename[0:2] == num of resets
    **  - data_filename[12:16] == adc read count
    */
    char data_filename[32];
    memset(data_filename, '\0', sizeof(data_filename));

    /*
    ** full_path[] will hold the path and filename together
    */
    char full_path[ OS_MAX_LOCAL_PATH_LEN ];
    memset(full_path, '\0', sizeof(full_path));

    /*
    ** Concatonate filename with ADC read count
    ** This will be where number of resets may be added to the front of the file name. 
    */
    if((os_ret_val = sprintf(data_filename, "%.3u%s%.5u%s", 0, "_t_", ads1115_adc_read_count, ".csv")) < 0)
    {
        OS_printf("ADS1115: sprintf returned [%d] expected non-negative\n", os_ret_val);
        OS_printf("ADS1115: target data_filename: \'%s\'\n", data_filename);
    }

    /*
    ** concatonate path and filename
    */
    if((os_ret_val = sprintf(full_path, "%s%s", virtual_path, data_filename)) < 0)
    {
        OS_printf("ADS1115: sprintf returned [%d] (expected non-negative)\n", os_ret_val);
        OS_printf("ADS1115: target full_path: \'%s\'\n", full_path);
    }

    OS_printf("ADS1115: Data File full_path[%d]: \'%s\'\n", sizeof(full_path), full_path);

    /*
    ** Create Data File
    ** returns file descriptor on success,
    ** exit on failure
    */
    if ((os_fd = OS_creat((const char * ) full_path, (int32) OS_READ_WRITE)) < OS_FS_SUCCESS)
    {
        OS_printf("ADS1115: ADC File Create Returned [%d] (expected non-negative value).\n", os_fd);
        //OS_printf("ADS1115: ADC File Create Returned [%s]\n", errname);
        return(-1);
    }

    /*
    ** 
    */
    char csv[3];
    if((os_ret_val = sprintf(csv, "%s", ", ") < 0))
    {
        OS_printf("ADS1115: sprintf returned [%d] expected non-negative\n", os_ret_val);
        OS_printf("ADS1115: target csv[%d]: \'%s\'\n", sizeof(csv), csv);
    }

    /*
    ** Write adc channel data to data file
    ** from start of channel data (&ADS1115_ChannelData.adc_ch_1)
    ** continuing 8 bytes (2 bytes per channel, 4 channels)
    */
    int adc_ch_sel = 0;
    uint16* adc_ch_buf;

    for(adc_ch_sel = 0; adc_ch_sel <= 3; adc_ch_sel++)
    {
        /*
        ** point buffer to channel data with index*2 offset
        */
        adc_ch_buf = (uint16 *)(&ADS1115_ChannelData.adc_ch_1 + (adc_ch_sel * ADS1115_ADC_CH_BUF_SIZE));

        /*
        ** Write voltage data from ADC to file
        */
        if ( (os_ret_val = OS_write(os_fd, (void *) adc_ch_buf, 2)) < 0 )
        {
            OS_printf("ADS1115: ADC Data OS_Write Failed, Retuned [%d] \n", os_ret_val);
            OS_close(os_fd);
            return(-1);
        }
        else
        {
            OS_printf("ADS1115: data written to file [%#2x]\n", (uint16 *) adc_ch_buf);
        }

        /*
        ** Write ', ' ([comma] and [space]) to format for CSV file
        */
        if ((adc_ch_sel != 3) && ((os_ret_val = OS_write(os_fd, (void *) &csv, 2)) < 0))
        {
            OS_printf("ADS1115: ADC Data OS_Write csv Failed, Retuned [%d] \n", os_ret_val);
            OS_close(os_fd);
            return(-1);
        }
    }

    OS_close(os_fd);

    return(0);
}