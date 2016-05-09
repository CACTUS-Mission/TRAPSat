/** @file tim_app.h
 *  Main header for tim_app.c. Includes headers, constants and prototypes.
 */

#ifndef _tim_app_h_
#define _tim_app_h_

// Required standard library headers for tim_app.c
#include <errno.h>
#include <string.h>
#include <unistd.h>

// Required non-standard-library headers for tim_app.c
#include <wiringPi.h>

// NASA Core Flight Executive (cFE) headers
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_es.h"
#include "cfe_evs.h"
#include "cfe_sb.h"

// NASA OS Abstraction Layer (OSAL) timer header
#include "osapi-os-timer.h"

/** Maximum number of messages to allow on a TIM-related cFE Software Bus pipe */
#define TIM_PIPE_DEPTH 32

/** Max length of the filename of an image taken by a camera */
#define TIM_MAX_IMAGE_NAME_LEN 15
/** Max length of the filename of a temperature CSV file */
#define TIM_MAX_TEMPS_NAME_LEN 16 /* 000_t_00000.csv */

/*
To be removed once confirmed
#define TIM_CHILD_TASK_NAME		          "TIM_CAMERAMAN"
#define TIM_CHILD_TASK_STACK_SIZE	 8192
#define TIM_CHILD_TASK_PRIORITY	     98
#define TIM_RASPICALL_MAX_LEN		     64
#define TIM_SYS_CALL_MAX_LEN		     TIM_RASPICALL_MAX_LEN +
OS_MAX_PATH_LEN
*/

/** The timer length (in microseconds) for the TIM power-off timer */
#define POWEROFF_TIME 345000000

/** The pin number for the TE-1 GPIO input */
#define TIM_MAIN_GPIO_PIN_TE 1
/** The pin number for the TE-1 signal output (parallel comms to rocket) */
#define TIM_PAR_GPIO_PIN_TE 30
/** The pin number for the parallel power */
#define TIM_PAR_GPIO_PIN_PWR 29

// Local function prototypes.
// Except for the entry point (tim_AppMain), these functions are not called from
// any other source module.
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

// OS_Timer callback funtion for poweroff
void timer_callback_poweroff_system(uint32 poweroff_timer_id);

boolean TIM_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif