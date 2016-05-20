/*******************************************************************************
** File: tim_app.h
**
** Purpose:
**   This file is main hdr file for the tim_app application.
**
**
*******************************************************************************/

#ifndef _tim_app_h_
#define _tim_app_h_

#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <wiringPi.h>

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "osapi-os-timer.h"



/***********************************************************************/

#define TIM_PIPE_DEPTH		        32

#define TIM_MAX_IMAGE_NAME_LEN      15
#define TIM_MAX_TEMPS_NAME_LEN      16 /* 000_t_00000.csv */

/*
To be removed once confirmed 
#define TIM_CHILD_TASK_NAME		          "TIM_CAMERAMAN"
#define TIM_CHILD_TASK_STACK_SIZE	 8192               
#define TIM_CHILD_TASK_PRIORITY	     98
#define TIM_RASPICALL_MAX_LEN		     64
#define TIM_SYS_CALL_MAX_LEN		     TIM_RASPICALL_MAX_LEN + OS_MAX_PATH_LEN
*/

#define POWEROFF_TIME 				               435000000 /* time before poweroff in microseconds */

/*
** TIM GPIO Pin Assignment for Parallel Status Update and TE-1 Event Detection
*/
#define TIM_MAIN_GPIO_PIN_TE     1
#define TIM_PAR_GPIO_PIN_TE	30
#define TIM_PAR_GPIO_PIN_PWR	29

/************************************************************************
** Type Definitions
*************************************************************************/


/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (tim_AppMain), these
**       functions are not called from any other source module.
*/
void TIM_AppMain(void);
void TIM_AppInit(void);
void TIM_ProcessCommandPacket(void);
void TIM_ProcessGroundCommand(void);
void TIM_ReportHousekeeping(void);
void TIM_ResetCounters(void);
void TIM_SendImageFile(void);
void TIM_SendTempsFile(void);
void TIM_SendLogFile(void);
void TIM_Parallel_Init(void);
void TIM_UpdateTimerEvent(void);

/*
** OS_Timer callback funtion for poweroff 
*/
void timer_callback_poweroff_system(uint32 poweroff_timer_id);




boolean TIM_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _tim_app_h_ */
