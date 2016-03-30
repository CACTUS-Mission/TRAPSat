/*
** Required Headers
*/
#include "vc0706_child.h"
/*
#include "vc0706_perfids.h"
#include "vc0706_msgids.h"
#include "vc0706_msg.h"
#include "vc0706_events.h"
#include "vc0706_version.h"
#include "vc0706_child.h"
*/
int VC0706_takePics(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* VC0706 child task -- startup initialization                     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 VC0706_ChildInit(void)
{
    char *TaskText = "VC0706 Child Task";
    int32 Result;

    /* Create child task - VC0706 monitor task */
    Result = CFE_ES_CreateChildTask(&VC0706_ChildTaskID,
                                     VC0706_CHILD_TASK_NAME,
                                     (void *) VC0706_ChildTask, 0,
                                     VC0706_CHILD_TASK_STACK_SIZE,
                                     VC0706_CHILD_TASK_PRIORITY, 0);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(VC0706_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
           "%s initialization error: create task failed: result = %d",
            TaskText, Result);
    }
    else
    {
        CFE_EVS_SendEvent(VC0706_CHILD_INIT_EID, CFE_EVS_INFORMATION,
           "%s initialization info: create task complete: result = %d",
            TaskText, Result);
    }


    if (Result != CFE_SUCCESS)
    {
    	return(Result);
    }

    return CFE_SUCCESS;

} /* End of VC0706_ChildInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* VC0706 child task -- task entry point                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void VC0706_ChildTask(void)
{
    char *TaskText = "VC0706 Child Task";
    int32 Result;

    /*
    ** The child task runs until the parent dies (normal end) or
    **  until it encounters a fatal error (semaphore error, etc.)...
    */
    Result = CFE_ES_RegisterChildTask();

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(VC0706_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
           "%s initialization error: register child failed: result = %d",
            TaskText, Result);
    }
    else
    {
        CFE_EVS_SendEvent(VC0706_CHILD_INIT_EID, CFE_EVS_INFORMATION,
           "%s initialization complete", TaskText);

        /*
        ** Child task process loop
        */
        VC0706_takePics();
        /*VC0706_ChildLoop();*/
    }

    /* This call allows cFE to clean-up system resources */
    CFE_ES_ExitChildTask();

    return;

} /* End of VC0706_ChildTask() */

