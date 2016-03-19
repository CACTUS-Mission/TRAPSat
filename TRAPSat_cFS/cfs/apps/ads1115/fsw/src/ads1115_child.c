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

extern ads1115_hk_tlm_t ADS1115_HkTelemetryPkt;
extern uint8 ads1115_childtask_read_once;

extern uint8 ads1115_adc_read_count;


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
        /*ADS1115_ReadADCChannelsLoop(); */
        ADS1115_ChildLoop();
    }

    /* This call allows cFE to clean-up system resources */
    CFE_ES_ExitChildTask();
 
   return;

} /* End of ADS1115_ADC_ChildTask() */

/*
** ADS1115 Infinite Child Loop
** 
** Check State
**  - Read ADC or not
** Loop
*/
void ADS1115_ChildLoop(void)
{
    int ret_val = 0; /* Buffer for ADC/OS call return values */
    ads1115_adc_read_count = 0; /* ADC Read Samples Counter */

    /*
    ** infinite read loop w/ 5 second delay
    ** Should never return from this loop during runtime
    */
    for ( ; ; )
    {
        /* 
        ** Clear ret before every ADC Read call 
        */
        ret_val = 0;

        /*
        ** Determine ADC Read Action based on childloop_state
        */
        switch(ADS1115_HkTelemetryPkt.ads1115_childloop_state)
        {
            case 0:     /* Infinite Read Loop */
                        if ((ret_val = ADS1115_ReadADCChannels()) < 0)
                        {
                            CFE_EVS_SendEvent(ADS1115_CHILD_ADC_ERR_EID,CFE_EVS_ERROR,
                                "ADC Read Ret Val=[%d]. Expected non-negative val.", ret_val);
                        }
                        else
                        {
                            /* Store Data */
                            if( (ret_val = ADS1115_StoreADCChannels()) < 0)
                            {
                                CFE_EVS_SendEvent(ADS1115_CHILD_ADC_ERR_EID,CFE_EVS_ERROR,
                                    "OS Store Ret Val=[%d]. Expected non-negative val.", ret_val);
                            }
                        }
                        break;

            case 1:     /* Read Once, Set Flag to not read again */
                        if(ads1115_childtask_read_once == 0)
                        {
                            if ((ret_val = ADS1115_ReadADCChannels()) < 0)
                            {
                                CFE_EVS_SendEvent(ADS1115_CHILD_ADC_ERR_EID,CFE_EVS_ERROR,
                                    "ADC Read Ret Val=[%d]. Expected non-negative val.", ret_val);
                            }
                            else
                            {
                                /* Set Flag */
                                ads1115_childtask_read_once = 1;

                                /* Store Data */
                                if( (ret_val = ADS1115_StoreADCChannels()) < 0)
                                {
                                    CFE_EVS_SendEvent(ADS1115_CHILD_ADC_ERR_EID,CFE_EVS_ERROR,
                                        "OS Store Ret Val=[%d]. Expected non-negative val.", ret_val);
                                }
                            }
                            
                        }
                        break;

            default:    /* Unknown State: Log it, Reset Flags, Continue */
                        CFE_EVS_SendEvent(ADS1115_CHILD_ADC_ERR_EID,CFE_EVS_ERROR,
                                    "ADC Child Loop State Argument [%d] unrecognized.", 
                                    ADS1115_HkTelemetryPkt.ads1115_childloop_state);
                        CFE_EVS_SendEvent(ADS1115_CHILD_ADC_ERR_EID,CFE_EVS_ERROR,
                                    "Resetting \'Child Loop State\' and \'Read Once Flag\'.");
                        ads1115_childtask_read_once = 0;
                        ADS1115_HkTelemetryPkt.ads1115_childloop_state = 0;
                        break;
        }

        /*
        ** Delay 5 Seconds (5000 milliseconds)
        */
        OS_printf("ADS1115: ADC Loop current state [%d]. Delay 5s.\n", ADS1115_HkTelemetryPkt.ads1115_childloop_state);
        OS_TaskDelay(5000);
        
    } /* Infinite ADC Read Loop End Here */

    return;
} /* End of: int ADS1115_ChildLoop(void) */