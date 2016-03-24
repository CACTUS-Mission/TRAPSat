/*
** $Id: tim_app_child.c krmoore Exp  $
**
** Purpose: TIM Child task
**
** Author: 
**
** Notes: 
**
** $Log: tim_app_child.c  $
*/

#include "cfe.h"
#include "tim_app.h"
#include "tim_app_perfids.h"
#include "tim_app_msgids.h"
#include "tim_app_msg.h"
#include "tim_app_events.h"
#include "tim_app_child.h"

#include <string.h>


/* 
 *  Variables from tim_app.c to be used in tim_app_child.c 
 */
extern TIM_SerialQueue_t   TIM_SerialQueueInfo;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TIM child task -- startup initialization                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 TIM_ChildInit(void)
{
	int32 Result;
	//int32 QueueResult;

	/* Create child task - TIM monitor task */
	Result = CFE_ES_CreateChildTask(&TIM_ChildTaskID,
					TIM_CHILD_TASK_NAME,
					(void *) TIM_ChildTask, 0,
					TIM_CHILD_TASK_STACK_SIZE,
					TIM_CHILD_TASK_PRIORITY, 0);

	if (Result != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(TIM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
		"Child initialization error: create task failed: result = %d", Result);
	}
	
	return(Result);

} /* End of TIM_ChildInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TIM child task -- task entry point                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void TIM_ChildTask(void)
{
	int32 Result;

	/*
	** The child task runs until the parent dies (normal end) or
	**  until it encounters a fatal error (semaphore error, etc.)...
	*/
	Result = CFE_ES_RegisterChildTask();

	if (Result != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(TIM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
		"Child Task initialization error: register child failed: result = %d", Result);
	}
	else
	{
		CFE_EVS_SendEvent(TIM_CHILD_INIT_EID, CFE_EVS_INFORMATION, "Child Task initialization complete");
		/* Child task process loop */
		TIM_ChildLoop();
	}

	/* This call allows cFE to clean-up system resources */
	CFE_ES_DeleteChildTask(TIM_ChildTaskID);

	return;

} /* End of TIM_ChildTask() */


void TIM_ChildLoop(void)
{
	for(;;)
	{
		OS_printf("\n\n");
		OS_printf("Inside TIM child loop\n");
		OS_printf("\n\n");
		OS_TaskDelay(5000);
	}
	return;
}
