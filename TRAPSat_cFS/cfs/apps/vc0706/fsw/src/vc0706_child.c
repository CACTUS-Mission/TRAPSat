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


extern VC0706_IMAGE_CMD_PKT_t VC0706_ImageCmdPkt;


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

int VC0706_SendTimFileName(char *file_name)
{
    CFE_SB_InitMsg((void *) &VC0706_ImageCmdPkt, (CFE_SB_MsgId_t) VC0706_IMAGE_CMD_MID, (uint16) VC0706_IMAGE_CMD_LNGTH, (boolean) 1 );

    int32 ret = CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &VC0706_ImageCmdPkt, (uint16) VC0706_IMAGE_CMD_CODE);

    if(ret < 0)
    {
        OS_printf("VC0706: SendTimFileName() Set Cmd Code Ret [%d].\n", ret);
    }

    //OS_printf("Copying filename [%s] into command packet.\n", file_name);
    snprintf(VC0706_ImageCmdPkt.ImageName, sizeof(VC0706_ImageCmdPkt.ImageName), "%s", file_name);
    //OS_printf("Command packet holds: [%s].\n", VC0706_ImageCmdPkt.ImageName);

    CFE_SB_GenerateChecksum((CFE_SB_MsgPtr_t) &VC0706_ImageCmdPkt);
    
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &VC0706_ImageCmdPkt);

    OS_printf("Message sent to TIM from VC0706.\n");    

    return 0;
}