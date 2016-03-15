/*******************************************************************************
** File: ads1115.h
**
** Purpose:
**   This file is main hdr file for the ADS1115 application.
**
**
*******************************************************************************/

#ifndef _ads1115_h_
#define _ads1115_h_

/*
** Platform Required header files
** Check necessity of these
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>

/*
** cFS Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

/*
** Application Required Header Files
*/


/***********************************************************************/

#define ADS1115_PIPE_DEPTH                     32


/*
** ADS1115 Child Task Definitions
*/
#define ADS1115_ADC_CHILD_TASK_NAME        "ADC_MONITOR"
#define ADS1115_ADC_CHILD_TASK_STACK_SIZE  8192               
#define ADS1115_ADC_CHILD_TASK_PRIORITY    200
#define ADS1115_ADDR                       0b01001001


/************************************************************************
** Type Definitions
*************************************************************************/

/*
** ADS1115 Channel Data Buffers
**	
** Each ADC channel measures a voltage (0V - 5V)
** The ADC converts the voltage reading to a 16-bit value
** between --> ((2^16) - 1).
*/
typedef struct
{
	uint8 adc_ch_0[2];
	uint8 adc_ch_1[2];
	uint8 adc_ch_2[2];
	uint8 adc_ch_3[2];
} ADS1115_Ch_Data_t;


/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (ADS1115_AppMain), these
**       functions are not called from any other source module.
*/
void ADS1115_AppMain(void);
void ADS1115_AppInit(void);
void ADS1115_ProcessCommandPacket(void);
void ADS1115_ProcessGroundCommand(void);
void ADS1115_ReportHousekeeping(void);
void ADS1115_ResetCounters(void);
void ADS1115_SetChildLoopDelay(void);

boolean ADS1115_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _ads1115_h_ */
