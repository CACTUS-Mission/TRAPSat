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

char num_reboots[3];

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
    
    
    /*
    ** Read number of reboots
    */
    setNumReboots();

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

    int32 ret = 0;
    
    /*
    ** Determine which camera is sending data, for Command Code determintation
    */
    if(file_name[4] == '0')
    {
        ret = CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &VC0706_ImageCmdPkt, (uint16) VC0706_IMAGE0_CMD_CODE);
    }
    else if (file_name[4] == '1')
    {
        ret = CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &VC0706_ImageCmdPkt, (uint16) VC0706_IMAGE1_CMD_CODE);
    }
    else
    {
        OS_printf("\tDid not recognize camera identifier in filename. Defaulting to Cam 0\n");
        ret = CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &VC0706_ImageCmdPkt, (uint16) VC0706_IMAGE0_CMD_CODE);
    }
    
    if(ret < 0)
    {
        OS_printf("VC0706: SendTimFileName() Set Cmd Code Ret [%d].\n", ret);
    }

    //OS_printf("Copying filename [%s] into command packet.\n", file_name);
    snprintf(VC0706_ImageCmdPkt.ImageName, sizeof(VC0706_ImageCmdPkt.ImageName), "%s", file_name);
    //OS_printf("Command packet holds: [%s].\n", VC0706_ImageCmdPkt.ImageName);

    CFE_SB_GenerateChecksum((CFE_SB_MsgPtr_t) &VC0706_ImageCmdPkt);
    
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &VC0706_ImageCmdPkt);

     CFE_EVS_SendEvent(VC0706_CHILD_INIT_INF_EID, CFE_EVS_INFORMATION, "Message sent to TIM from VC0706.");

    return 0;
}

void setNumReboots(void)
{
    //OS_printf("Inside VC get reboot\n");

    memset(num_reboots, '0', sizeof(num_reboots));

    mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IROTH;

    int32 fd = OS_open((const char *) "/ram/logs/reboot.txt", (int32) OS_READ_ONLY, (uint32) mode);

    if(fd < OS_FS_SUCCESS)
    {
        OS_printf("\tCould not open reboot file in VC, ret = %d!\n", fd);
    }

    int os_ret = OS_read((int32) fd, (void *) num_reboots, (uint32) 3);

    if( os_ret < OS_FS_SUCCESS)
    {
        memset(num_reboots, '9', sizeof(num_reboots));
        OS_printf("\tCould not read from reboot file in VC, ret = %d!\n", os_ret);
    }

    OS_close(fd);
}

