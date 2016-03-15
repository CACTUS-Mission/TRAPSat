/*
** Required Headers
*/
#include "ads1115.h"
#include "ads1115_perfids.h"
#include "ads1115_msgids.h"
#include "ads1115_msg.h"
#include "ads1115_events.h"
#include "ads1115_version.h"
#include "ads1115_child.h"

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
        ADS1115_ReadADCChannelsLoop();
        /*ADS1115_ChildLoop();*/
    }

    /* This call allows cFE to clean-up system resources */
    CFE_ES_ExitChildTask();

    return;

} /* End of ADS1115_ADC_ChildTask() */




/*
** ADS1115 Child Loop
** 
** Determine Delay/Action
** Read ADC
** Loop
*/
int ADS1115_ChildLoop(void)
{
    /*
    ** infinite read loop
    ** w/ 5 second delay
    */
    for ( ; ; )
    {
        /*
        ** Control Loop Mechanism Here
        ** based on childloop_state
        */
        switch(ADS1115_HkTelemetryPkt.ads1115_childloop_state)
        {
            case 0:     /* Infinite Update Loop */
                        ADS1115_ReadADCChannels();
                        break;
            case 1:     /* Read Once */
                        if(it not set)
                        {
                            ADS1115_ReadADCChannels();
                            set it;
                        }
                        break;
            default:    continue;
                        break;
        }

        /*
        ** End control loop delay mech
        */

        OS_TaskDelay(5000);

        
    } /* Infinite ADC Read Loop End Here */

    return(0);
}