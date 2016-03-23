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

#define ADS1115_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

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

boolean ADS1115_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _ads1115_h_ */