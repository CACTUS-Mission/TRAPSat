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
extern ads1115_hk_tlm_t   ADS1115_HkTelemetryPkt;
//extern uint32		       Cameraman_QueueID;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* TIM child task -- startup initialization                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 TIM_ChildInit(void)
{
	char *TaskText = "Cameraman Task";
	char *QueueTaskText = "Cameraman Queue";
	int32 Result;
	int32 QueueResult;

	/* Create child task - TIM monitor task */
	Result = CFE_ES_CreateChildTask(&TIM_ChildTaskID,
					TIM_CHILD_TASK_NAME,
					(void *) TIM_ChildTask, 0,
					TIM_CHILD_TASK_STACK_SIZE,
					TIM_CHILD_TASK_PRIORITY, 0);
	if (Result != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(TIM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
		"%s initialization error: create task failed: result = %d",
		TaskText, Result);
	}
	
	/* Create Cameraman Queue */
	QueueResult = OS_QueueCreate( &TIM_SerialQueueInfo.serial_qid,
					"Cameraman_QueueName",
					TIM_QUEUE_MAX_LEN,
					TIM_SYS_CALL_MAX_LEN,
					0 );
	if (QueueResult != OS_SUCCESS)
	{
		CFE_EVS_SendEvent(TIM_QUEUE_CREATE_ERR_EID, CFE_EVS_ERROR,
		"%s initialization error: create task failed: result = %d",
		QueueTaskText, QueueResult);
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
	char *TaskText = "Cameraman Task";
	int32 Result;

	/*
	** The child task runs until the parent dies (normal end) or
	**  until it encounters a fatal error (semaphore error, etc.)...
	*/
	Result = CFE_ES_RegisterChildTask();

	if (Result != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(TIM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
		"%s initialization error: register child failed: result = %d",
		TaskText, Result);
	}
	else
	{
		CFE_EVS_SendEvent(TIM_CHILD_INIT_EID, CFE_EVS_INFORMATION,
				"%s initialization complete", TaskText);
		/* Child task process loop */
		TIM_ChildLoop();
	}

	/* This call allows OSAL to free queue id for later use */
	/* OS_QueueDelete(Cameraman_QueueID); */
	
	/* This call allows cFE to clean-up system resources */
	CFE_ES_DeleteChildTask(TIM_ChildTaskID);

	return;

} /* End of TIM_ChildTask() */


void TIM_ChildLoop(void)
{
	int32 QueueResult;
	for(;;)
	{
		char full_sys_call[ TIM_SYS_CALL_MAX_LEN ];
		uint32 size_copied;
		
//		/* Pull From Queue */
//		QueueResult = OS_QueueGet( TIM_SerialQueueInfo.serial_qid,
//						full_sys_call,
//						TIM_SYS_CALL_MAX_LEN,
//						&size_copied,
//						OS_PEND );
//		if ( QueueResult == OS_SUCCESS )
//		{
//			/* Decrement Queue Counter */					
//			TIM_SerialQueueInfo.on_queue--;
//				
//			/* 
//			** Process system calls from queue
//			** set cam_status true while camera is active
//			*/
//			TIM_SerialQueueInfo.serial_status = 1;
//			OS_printf("System Call Shown Below:\n%s\n", full_sys_call);
//			system(full_sys_call);
//			TIM_SerialQueueInfo.serial_status = 0;
//
//			/*
//			** Parse the command argument and the filepath argument
//			** "raspistill" or "raspivid" expected to be first string
//			** sscanf skips everything after first string until it gets
//			** to the last string, starting at '.'
//			*/
//			char command[ 15 ]; 
//			char filepath[ OS_MAX_PATH_LEN ];
//			sscanf(full_sys_call, "%s %*[^'.'] %s", command, filepath);
//			
//			if ( strcmp(command, "raspistill") == 0 )
//			{
//				/* Move picture path to HouseKeeping Packet */
//				memcpy(TIM_HkTelemetryPkt.tim_last_image_sent, filepath, OS_MAX_PATH_LEN);
//
//				char *TaskText = "Cameraman Proc: raspistill";
//				CFE_EVS_SendEvent(TIM_COMMAND_PIC_EID, CFE_EVS_INFORMATION,
//				"%s : Complete", TaskText);
//			}
//			else if ( strcmp(command, "raspivid") == 0 )
//			{
//				/* Move video path to HouseKeeping Packet */
//				memcpy(TIM_HkTelemetryPkt.tim_last_image_sent, filepath, OS_MAX_PATH_LEN);
//
//				char *TaskText = "Cameraman Proc: raspivid";
//				CFE_EVS_SendEvent(TIM_COMMAND_VID_EID, CFE_EVS_INFORMATION,
//				"%s : Complete", TaskText);
//			}	
//			else
//			{
//				char *TaskText = "Cameraman Proc: UNKNOWN";
//				int32 TaskResult = 0;
//				CFE_EVS_SendEvent(TIM_CHILD_UNKWN_ERR_EID, CFE_EVS_ERROR,
//				"%s in TIM_ChildLoop(): Check execution success : result = %d",
//				TaskText, TaskResult);
//			}
//		}
//		else
//		{
//			switch (QueueResult)
//			{
//				case OS_ERR_INVALID_ID:
//					OS_printf("Cameraman OS_QueueGet error: OS_ERR_INVALID_ID\n");
//					break;
//				case OS_INVALID_POINTER:
//					OS_printf("Cameraman OS_QueueGet error: OS_ERR_INVALID_ID\n");
//					break;
//				case OS_QUEUE_TIMEOUT:
//					OS_printf("Cameraman OS_QueueGet error: OS_ERR_INVALID_ID\n");
//					break;
//				case OS_QUEUE_INVALID_SIZE:
//					OS_printf("Cameraman OS_QueueGet error: OS_ERR_INVALID_ID\n");
//					break;
//				case OS_QUEUE_EMPTY:
//					OS_printf("Cameraman OS_QueueGet error: OS_QUEUE_EMPTY\n");
//					break;
//				default: /* All cases should be covered */
//					OS_printf("Cameraman QueueResult not what expected!");
//			}	
//			/* Repeat Pend after 10 seconds */
//			OS_TaskDelay(10000);
//		}
		OS_printf("\n\n");
		OS_printf("Inside TIM child loop\n");
		OS_printf("\n\n");
		OS_TaskDelay(5000);
	}
	return;
}
