/*
** $Id: tim_app_child.h  $
**
** Title: TIM CAMERAMAN (TIM) Child Task Header File
**
** Purpose: Prototypes for child task functions.
**
** Author: 
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
** $Log: tim_app_child.h  $
**
*/

#ifndef _tim_app_child_h_
#define _tim_app_child_h_

#include "cfe.h"

extern tim_hk_tlm_t       TIM_HkTelemetryPkt;
extern uint32               TIM_ChildTaskID;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TIM child task global function prototypes                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Child Task Initialization Function
**
**  \par Description
**       This function is invoked during TIM application startup initialization to
**       create and initialize the TIM Child Task.  The purpose for the child task
**       is to queue the latest jobs for the TIM "Cameraman".  
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
**  \sa #TIM_AppInit
**/
int32 TIM_ChildInit(void);


/**
**  \brief Child Task Entry Point Function
**
**  \par Description
**       This function is the entry point for the TIM application child task.  The
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
void TIM_ChildTask(void);

/**
**  \brief Child Task Main Loop Processor Function
**
**  \par Description
**       This function is the main loop for the TIM application child task. 
**
**  \par Assumptions, External Events, and Notes:
**	 
**  \param [in]
**	 
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**/
void TIM_ChildLoop(void);

#endif /* _tim_app_child_h_ */

/************************/
/*  End of File Comment */
/************************/

