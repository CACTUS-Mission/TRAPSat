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
