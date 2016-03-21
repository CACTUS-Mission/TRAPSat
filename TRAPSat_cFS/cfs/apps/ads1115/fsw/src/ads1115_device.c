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
extern ads1115_hk_tlm_t ADS1115_HkTelemetryPkt;
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
    ** Clear previous data in the struct (for debugging)
    **
    ** we should remove this during flight!
    */
    memset(&ADS1115_ChannelData.adc_ch_0[0], 0, 8);

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

        OS_printf("Before I2C Read:\n");
        OS_printf("i2c_data[%d]: *[%u] = [%#.2X]\n", 0, &i2c_data[0], i2c_data[0]);
        OS_printf("i2c_data[%d]: *[%u] = [%#.2X]\n", 1, &i2c_data[1], i2c_data[1]);
        
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
        
        /*
        ** Read 2 bytes into the i2c_data uint8 array
        **
        ** Note: big-endian format
        ** First byte is MSB of ADC conversion register
        ** Second byte is LSB of ADC conversion register
        */
        if ((io_res = read(i2c_fd, i2c_data, 2)) < 0)
        {
            OS_printf( "ADS1115: I2C Read Failure: Returned %d\n", io_res);
        }
        
        /*
        OS_printf("After I2C Read:\n");
        OS_printf("i2c_data[%d]: *[%u] = [%#.2X]\n", 0, &i2c_data[0], i2c_data[0]);
        OS_printf("i2c_data[%d]: *[%u] = [%#.2X]\n", 1, &i2c_data[1], i2c_data[1]);
        */


        OS_printf("ADS1115: ADC Channel [%d] Conversion Register MSB: [%#.2X] \n", adc_ch_sel, i2c_data[0]);
        OS_printf("ADS1115: ADC Channel [%d] Conversion Register LSB: [%#.2X] \n", adc_ch_sel, i2c_data[1]);


        /*
        ** Section 1
        ** This Section COULD be removed before launch to save processor time,
        ** OR we can keep it to have the voltage printout in our logs.
        */

        uint16 i2c_data_word;
        
        //OS_printf("Before pack:\n");
        //OS_printf("i2c_data_word: [%u], [%#.4X]\n", &i2c_data_word, i2c_data_word);
        

        i2c_data_word = i2c_data[0] << 8 | i2c_data[1];
        
        //OS_printf("After pack:\n");
        //OS_printf("i2c_data_word: *[%u] = [%#.4X]\n", &i2c_data_word, i2c_data_word);

        OS_printf("ADS1115: ADC Channel [%d] Voltage: %f V \n", adc_ch_sel, (float) i2c_data_word*4.096/32767.0);
        
        /*
        ** End of Section 1
        */

        /*
        OS_printf("Before copy:\n");
        OS_printf("ADS1115_ChannelData.adc_ch_0[0]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_0[0], ADS1115_ChannelData.adc_ch_0[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_0[1]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_0[1], ADS1115_ChannelData.adc_ch_0[1]);
        OS_printf("ADS1115_ChannelData.adc_ch_1[0]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_1[0], ADS1115_ChannelData.adc_ch_1[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_1[1]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_1[1], ADS1115_ChannelData.adc_ch_1[1]);
        OS_printf("ADS1115_ChannelData.adc_ch_2[0]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_2[0], ADS1115_ChannelData.adc_ch_2[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_2[1]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_2[1], ADS1115_ChannelData.adc_ch_2[1]);
        OS_printf("ADS1115_ChannelData.adc_ch_3[0]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_3[0], ADS1115_ChannelData.adc_ch_3[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_3[1]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_3[1], ADS1115_ChannelData.adc_ch_3[1]);
        */
        
        /*
        OS_printf("sizeof(ADS1115_ChannelData.adc_ch_1) = %d\n", sizeof(ADS1115_ChannelData.adc_ch_1));
        OS_printf("ADS1115_ADC_CH_BUF_SIZE = %d\n", ADS1115_ADC_CH_BUF_SIZE);
        OS_printf("adc_ch_sel = %d\n", adc_ch_sel);
        OS_printf("memcpy dest addr = %u\n", (&ADS1115_ChannelData.adc_ch_0 + (ADS1115_ADC_CH_BUF_SIZE*adc_ch_sel)));
        OS_printf("sizeof(memcpy dest) = %d\n", sizeof((&ADS1115_ChannelData.adc_ch_0 + (ADS1115_ADC_CH_BUF_SIZE*adc_ch_sel))));
        OS_printf("sizeof(&i2c_data) = %d\n", sizeof(&i2c_data));
        */

        /*
        ** Copy Data to ChannelData struct
        **
        */
        memcpy((&ADS1115_ChannelData.adc_ch_0 + adc_ch_sel), &i2c_data, ADS1115_ADC_CH_BUF_SIZE);

        /*
        OS_printf("Data after copy from i2c_data to ADS1115_ChannelData:\n");
        OS_printf("ADS1115_ChannelData.adc_ch_0[0]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_0[0], ADS1115_ChannelData.adc_ch_0[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_0[1]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_0[1], ADS1115_ChannelData.adc_ch_0[1]);
        OS_printf("ADS1115_ChannelData.adc_ch_1[0]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_1[0], ADS1115_ChannelData.adc_ch_1[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_1[1]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_1[1], ADS1115_ChannelData.adc_ch_1[1]);
        OS_printf("ADS1115_ChannelData.adc_ch_2[0]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_2[0], ADS1115_ChannelData.adc_ch_2[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_2[1]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_2[1], ADS1115_ChannelData.adc_ch_2[1]);
        OS_printf("ADS1115_ChannelData.adc_ch_3[0]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_3[0], ADS1115_ChannelData.adc_ch_3[0]);
        OS_printf("ADS1115_ChannelData.adc_ch_3[1]: *[%u] = [%#.2X]\n", &ADS1115_ChannelData.adc_ch_3[1], ADS1115_ChannelData.adc_ch_3[1]);
        */

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
    if((os_ret_val = snprintf(virtual_path, sizeof(virtual_path), "%s", "/ram/temps/") < 0))
    {
        OS_printf("ADS1115: snprintf returned [%d] expected non-negative\n", os_ret_val);
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
    char full_path[ OS_MAX_PATH_LEN ];
    memset(full_path, '\0', sizeof(full_path));

    /*
    ** Concatonate filename with ADC read count
    ** This will be where number of resets may be added to the front of the file name. 
    */
    if((os_ret_val = snprintf(data_filename, sizeof(data_filename), "%.3u%s%.5u%s", 0, "_t_", ads1115_adc_read_count, ".csv")) < 0)
    {
        OS_printf("ADS1115: snprintf returned [%d] expected non-negative\n", os_ret_val);
        OS_printf("ADS1115: target data_filename: \'%s\'\n", data_filename);
    }

    /*
    ** concatonate path and filename
    */
    if((os_ret_val = snprintf(full_path, sizeof(full_path), "%s%s", virtual_path, data_filename)) < 0)
    {
        OS_printf("ADS1115: snprintf returned [%d] (expected non-negative)\n", os_ret_val);
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
    ** Write adc channel data to data file
    ** from start of channel data (&ADS1115_ChannelData.adc_ch_0)
    ** continuing 8 (2 bytes per channel, 4 channels)
    */
    int adc_ch_sel = 0;

    /*
    ** Data buffer for file write
    ** file_data_buff[0] = 0; // ADC Conversion Register MSB
    ** file_data_buff[1] = 0; // ADC Conversion Register LSB
    ** file_data_buff[2] = ',';
    ** file_data_buff[3] = ' ';
    */
    uint8 file_data_buff[4];

    file_data_buff[0] = 0;
    file_data_buff[1] = 0;
    file_data_buff[2] = ',';
    file_data_buff[3] = ' ';

    for(adc_ch_sel = 0; adc_ch_sel <= 3; adc_ch_sel++)
    {
        OS_printf("Writting channel [%d] data to file.\n", adc_ch_sel);

        file_data_buff[0] = 0;
        file_data_buff[1] = 0;

        /*
        OS_printf("\n");
        OS_printf("&adc_data_buf[0] = [%u], file_data_buff[0] = [%#.2X]\n", &file_data_buff[0], file_data_buff[0]);
        OS_printf("&adc_data_buf[1] = [%u], file_data_buff[1] = [%#.2X]\n", &file_data_buff[1], file_data_buff[1]);
        OS_printf("&adc_data_buf[2] = [%u], file_data_buff[2] = [%#.2X]\n", &file_data_buff[2], file_data_buff[2]);
        OS_printf("&adc_data_buf[3] = [%u], file_data_buff[3] = [%#.2X]\n", &file_data_buff[3], file_data_buff[3]);
        */

        file_data_buff[0] = (uint8) *(&ADS1115_ChannelData.adc_ch_0[0] + (adc_ch_sel*ADS1115_ADC_CH_BUF_SIZE));
        file_data_buff[1] = (uint8) *(&ADS1115_ChannelData.adc_ch_0[1] + (adc_ch_sel*ADS1115_ADC_CH_BUF_SIZE));
        file_data_buff[2] = ',';
        file_data_buff[3] = ' ';

        OS_printf("ADC Data to be written to file:\n");
        OS_printf("Byte 1: file_data_buff[0] = [%#.2X]\n", file_data_buff[0]);
        OS_printf("Byte 2: file_data_buff[1] = [%#.2X]\n", file_data_buff[1]);
        OS_printf("Byte 3: file_data_buff[2] = [%#.2X]\n", file_data_buff[2]);
        OS_printf("Byte 4: file_data_buff[3] = [%#.2X]\n", file_data_buff[3]);

        /*
        ** Write voltage data from ADC to file
        */
        if ((os_ret_val = OS_write(os_fd, (void *) file_data_buff, 4)) < 0)
        {
            OS_printf("ADS1115: ADC Data OS_Write Failed, Retuned [%d] \n", os_ret_val);
            OS_close(os_fd);
            return(-1);
        }
    }

    OS_close(os_fd);

    //strncpy(ADS1115_HkTelemetryPkt.ads1115_datafilepath, full_path, sizeof(ADS1115_HkTelemetryPkt.ads1115_datafilepath));

    if((os_ret_val = snprintf(ADS1115_HkTelemetryPkt.ads1115_datafilepath, sizeof(ADS1115_HkTelemetryPkt.ads1115_datafilepath), "%s", full_path)) < 0)
    {
        OS_printf("ADS1115: Failed moving filepath to HK Packet. snprintf ret = [%d]\n", os_ret_val);
    }

    return(0);
}