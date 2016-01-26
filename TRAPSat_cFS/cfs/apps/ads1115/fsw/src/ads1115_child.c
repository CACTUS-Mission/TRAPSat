/*
**
*/

#include "cfe.h"
#include "ads1115.h"
#include "ads1115_perfids.h"
#include "ads1115_msgids.h"
#include "ads1115_msg.h"
#include "ads1115_events.h"
#include "ads1115_child.h"

#include <string.h>

int ADS1115_ReadADCChannels(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* ADS1115 child task -- startup initialization                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 ADS1115_ChildInit(void)
{
    char *TaskText = "ADC Child Task";
    int32 Result;

    /* Create child task - ADS1115 monitor task */
    Result = CFE_ES_CreateChildTask(&ADS1115_ADC_ChildTaskID,
                                     ADS1115_ADC_CHILD_TASK_NAME,
                                     (void *) ADS1115_ADC_ChildTask, 0,
                                     ADS1115_ADC_CHILD_TASK_STACK_SIZE,
                                     ADS1115_ADC_CHILD_TASK_PRIORITY, 0);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(ADS1115_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
           "%s initialization error: create task failed: result = %d",
            TaskText, Result);
    }
    else
    {
        CFE_EVS_SendEvent(ADS1115_CHILD_INIT_EID, CFE_EVS_INFORMATION,
           "%s initialization info: create task complete: result = %d",
            TaskText, Result);
    }


    if (Result != CFE_SUCCESS)
    {
    	return(Result);
    }

    return CFE_SUCCESS;

} /* End of ADS1115_ChildInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* ADC child task -- task entry point                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void ADS1115_ADC_ChildTask(void)
{
    char *TaskText = "ADS1115 Child Task";
    int32 Result;

    /*
    ** The child task runs until the parent dies (normal end) or
    **  until it encounters a fatal error (semaphore error, etc.)...
    */
    Result = CFE_ES_RegisterChildTask();

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(ADS1115_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
           "%s initialization error: register child failed: result = %d",
            TaskText, Result);
    }
    else
    {
        CFE_EVS_SendEvent(ADS1115_CHILD_INIT_EID, CFE_EVS_INFORMATION,
           "%s initialization complete", TaskText);

        /* 
        ** Child task process loop
        */
        ADS1115_ReadADCChannels();
        /*ADS1115_ChildLoop();*/
    }

    /* This call allows cFE to clean-up system resources */
    CFE_ES_ExitChildTask();

    return;

} /* End of ADS1115_ADC_ChildTask() */

