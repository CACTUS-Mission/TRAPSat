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

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "vc0706_core.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

/***********************************************************************/

#define VC0706_PIPE_DEPTH                     32

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
