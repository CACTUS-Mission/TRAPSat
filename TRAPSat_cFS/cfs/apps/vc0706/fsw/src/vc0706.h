/*******************************************************************************
** File: vc0706.h
**
** Purpose:
**   This file is main hdr file for the VC0706 application.
**
**
*******************************************************************************/

#ifndef _vc0706_h_
#define _vc0706_h_


#define MUX_SEL_PIN				13	// GPIO pin for SN74 Select line
#define LED_PIN					16	// GPIO pin for LEDs
#define VC0706_MAX_FILENAME_LEN 24  // Maximum expected filename length /ram/images/<reboots [3 char]>_<cam 0 or 1 [1 char]>_<filenum [3 char]>.jpg

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"


/*
** App specific files
*/
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wiringSerial.h>
#include <wiringPi.h>

#include "vc0706_led.h"
#include "vc0706_mux.h"

#include "vc0706_core.h"

#include "vc0706_perfids.h"
#include "vc0706_msgids.h"
#include "vc0706_msg.h"
#include "vc0706_events.h"
#include "vc0706_version.h"

/***********************************************************************/

#define VC0706_PIPE_DEPTH                     32

/*
** VC0706 Child Task Definitions
*/
#define VC0706_CHILD_TASK_NAME        "CAMERA_CONTROL"
#define VC0706_CHILD_TASK_STACK_SIZE  8192
#define VC0706_CHILD_TASK_PRIORITY    200


/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (VC0706_AppMain), these
**       functions are not called from any other source module.
*/
void VC0706_AppMain(void);
void VC0706_AppInit(void);
void VC0706_ProcessCommandPacket(void);
void VC0706_ProcessGroundCommand(void);
void VC0706_ReportHousekeeping(void);
void VC0706_ResetCounters(void);

boolean VC0706_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _vc0706_h_ */
