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

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"



#include <string.h>
#include <errno.h>
#include <unistd.h>

/***********************************************************************/

#define TIM_PIPE_DEPTH		        32
#define TIM_CHILD_TASK_NAME		    "TIM_CAMERAMAN"
#define TIM_CHILD_TASK_STACK_SIZE	8192               
#define TIM_CHILD_TASK_PRIORITY	    98
#define TIM_RASPICALL_MAX_LEN		64
#define TIM_SYS_CALL_MAX_LEN		TIM_RASPICALL_MAX_LEN + OS_MAX_PATH_LEN

/*
** This is the max amount of 
*/
#define TIM_QUEUE_MAX_LEN		    10

/************************************************************************
** Type Definitions
*************************************************************************/

/*
 * tim_Cameraman_t
 *  uint8 on_queue    ~   stores the number of commands
 *                        to be processed by the childtask
 *                        (the number of commands on the queue)	
 *  uint8 serial_status  ~   status flag should be
 *                        1 - if serial is in use
 *                        0 - if serial is not in use
 */
typedef struct
{
	uint8	on_queue;
	uint8	cam_status;
} tim_Cameraman_t;


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

boolean TIM_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _tim_app_h_ */
