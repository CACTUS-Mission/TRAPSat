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

#define tim_PIPE_DEPTH		        32
#define tim_CHILD_TASK_NAME		    "tim_CAMERAMAN"
#define tim_CHILD_TASK_STACK_SIZE	8192               
#define tim_CHILD_TASK_PRIORITY	    98
#define tim_RASPICALL_MAX_LEN		64
#define tim_SYS_CALL_MAX_LEN		tim_RASPICALL_MAX_LEN + OS_MAX_PATH_LEN

/*
** This is the max amount of 
*/
#define tim_QUEUE_MAX_LEN		    10

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
void tim_AppMain(void);
void tim_AppInit(void);
void tim_ProcessCommandPacket(void);
void tim_ProcessGroundCommand(void);
void tim_ReportHousekeeping(void);
void tim_ResetCounters(void);
void tim_SendImageFile(void);
void tim_SendTempsFile(void);

boolean tim_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _tim_app_h_ */
