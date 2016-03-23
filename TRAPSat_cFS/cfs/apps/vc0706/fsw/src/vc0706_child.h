

#ifndef _vc0706_child_h_
#define _vc0706_child_h_

/*
** VC0706 Master Header
*/
#include "vc0706.h"

/*
** Platform Required header files
*/
/*
#include <stdio.h>   // Standard input/output definitions 
#include <fcntl.h>   // File control definitions 
#include <termios.h> // POSIX terminal control definitions 
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
*/

/* Check necessity of each of these to trim fat */

extern vc0706_hk_tlm_t       VC0706_HkTelemetryPkt;
extern uint32                VC0706_ChildTaskID;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* VC0706 child task global function prototypes                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Child Task Initialization Function
**
**  \par Description
**       This function is invoked during VC0706 application startup initialization to
**       create and initialize the Camera Child Task.  The purpose for the child task
**       is to send commands to the camera, and to recieve then store images taken.  
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS     \endcode
**  \retstmt Error return codes from #CFE_ES_CreateChildTask \endcode
**  \endreturns
**
**  \sa #VC0706_AppInit
**/
int32 VC0706_ChildInit(void);


/**
**  \brief Child Task Entry Point Function
**
**  \par Description
**       This function is the entry point for the VC0706 application child task.  The
**       function registers with CFE as a child task, 
**       interface with the parent task and calls the child task main loop function.
**       Should the main loop function return due to a breakdown in the interface
**       handshake with the parent task, this function will self delete as a child
**       task with CFE. There is no return from #CFE_ES_DeleteChildTask.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #CFE_ES_DeleteChildTask, #FM_ChildLoop
**/
void VC0706_ChildTask(void);


/**
**  \brief Child Task Main Loop Processor Function
**
**  \par Description
**       This function is the main loop for the VC0706 application child task. 
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**/
void VC0706_ChildLoop(void);


#endif /* _vc0706_child_h_ */

/************************/
/*  End of File Comment */
/************************/

