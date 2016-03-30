/*******************************************************************************
** File: tim_app.c
**
** Purpose:
**   This file contains the source code for the TIM App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "tim_app.h"
#include "tim_app_perfids.h"
#include "tim_app_msgids.h"
#include "tim_app_msg.h"
#include "tim_app_events.h"
#include "tim_app_child.h"
#include "tim_app_version.h"



/*
** global data
*/

tim_hk_tlm_t       TIM_HkTelemetryPkt;
CFE_SB_PipeId_t    TIM_CommandPipe;
CFE_SB_MsgPtr_t    TIMMsgPtr;

/*
** global data for child task (cameraman) use
*/
uint32             	TIM_ChildTaskID;
TIM_Cameraman_t	    TIM_CameramanPkt;
uint32			    Cameraman_QueueID;

static CFE_EVS_BinFilter_t  TIM_EventFilters[] =
       {  /* Event ID    mask */
          {TIM_STARTUP_INF_EID,       0x0000},
          {TIM_COMMAND_ERR_EID,       0x0000},
          {TIM_COMMANDNOP_INF_EID,    0x0000},
          {TIM_COMMANDRST_INF_EID,    0x0000},
       };

/*
** Prototype Section
*/


/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* TIM_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void TIM_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(TIM_APP_PERF_ID);

    TIM_AppInit();

    /*
    ** TIM Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(TIM_APP_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&TIMMsgPtr, TIM_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(TIM_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            TIM_ProcessCommandPacket();
        }
    }

    CFE_ES_ExitApp(RunStatus);

} /* End of TIM_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* TIM_AppInit() --  initialization                                         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void TIM_AppInit(void)
{
	/*
	** Register the app with Executive services
	*/
	CFE_ES_RegisterApp() ;

	/*
	** Register the events
	*/ 
	CFE_EVS_Register(TIM_EventFilters,
			sizeof(TIM_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
			CFE_EVS_BINARY_FILTER);

	/*
	** Create the Software Bus command pipe and subscribe to housekeeping
	**  messages
	*/
	CFE_SB_CreatePipe(&TIM_CommandPipe, TIM_PIPE_DEPTH,"TIM_CMD_PIPE");
	CFE_SB_Subscribe(TIM_APP_CMD_MID, TIM_CommandPipe);
	CFE_SB_Subscribe(TIM_APP_SEND_HK_MID, TIM_CommandPipe);

	TIM_ResetCounters();

	/* Clear the Camera Queue */
	TIM_CameramanPkt.on_queue = 0;	
	
	TIM_ChildInit();

	CFE_SB_InitMsg(&TIM_HkTelemetryPkt,
			TIM_APP_HK_TLM_MID,
			TIM_APP_HK_TLM_LNGTH, TRUE);

	CFE_EVS_SendEvent (TIM_STARTUP_INF_EID, CFE_EVS_INFORMATION,
			"TIM App Initialized. Version %d.%d.%d.%d",
			TIM_APP_MAJOR_VERSION,
			TIM_APP_MINOR_VERSION, 
			TIM_APP_REVISION, 
			TIM_APP_MISSION_REV);
				
} /* End of TIM_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TIM_ProcessCommandPacket                                         */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the TIM     */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TIM_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(TIMMsgPtr);

    switch (MsgId)
    {
        case TIM_APP_CMD_MID:
            TIM_ProcessGroundCommand();
            break;

        case TIM_APP_SEND_HK_MID:
            TIM_ReportHousekeeping();
            break;

        default:
            TIM_HkTelemetryPkt.tim_command_error_count++;
            CFE_EVS_SendEvent(TIM_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"TIM: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End TIM_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TIM_ProcessGroundCommand() -- TIM ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void TIM_ProcessGroundCommand(void)
{
	uint16 CommandCode;

	CommandCode = CFE_SB_GetCmdCode(TIMMsgPtr);
	

	/* Process "known" TIM app ground commands */
	switch (CommandCode)
	{
	case TIM_APP_NOOP_CC:
		TIM_HkTelemetryPkt.tim_command_count++;
		CFE_EVS_SendEvent(TIM_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"TIM: NOOP command");
		break;

	case TIM_APP_RESET_COUNTERS_CC:
		TIM_ResetCounters();
		break;
	
	case TIM_APP_SEND_IMAGE_CC:
		TIM_SendImageFile();
		CFE_EVS_SendEvent(TIM_COMMAND_PIC_EID,CFE_EVS_INFORMATION, 
					"TIM: Send Image File Command Received");
		TIM_HkTelemetryPkt.tim_command_count++;
		TIM_HkTelemetryPkt.tim_command_image_count++;
		TIM_CameramanPkt.on_queue++;
		break;

	case TIM_APP_SEND_TEMPS_CC:
		TIM_SendTempsFile();
		CFE_EVS_SendEvent(TIM_COMMAND_VID_EID,CFE_EVS_INFORMATION,
					 "TIM: Send Temps File Command Received");
		TIM_HkTelemetryPkt.tim_command_count++;
		TIM_HkTelemetryPkt.tim_command_video_count++;
		TIM_CameramanPkt.on_queue++;
		break;

	/* default case already found during FC vs length test */
	default:
		CFE_EVS_SendEvent(TIM_COMMAND_ERR_EID, CFE_EVS_INFORMATION,
				"Command code not found (CC = %i)", CommandCode);
		TIM_HkTelemetryPkt.tim_command_error_count++;
		break;
    }
    return;

} /* End of TIM_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TIM_ReportHousekeeping                                           */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TIM_ReportHousekeeping(void)
{
	memcpy((char *) &TIM_HkTelemetryPkt.CameraInfo,
		(char *) &TIM_CameramanPkt,
		sizeof(TIM_Cameraman_t));

	CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &TIM_HkTelemetryPkt);
	CFE_SB_SendMsg((CFE_SB_Msg_t *) &TIM_HkTelemetryPkt);
	return;

} /* End of TIM_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TIM_ResetCounters                                                */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry. Also clears file loc data.             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TIM_ResetCounters(void)
{
	/* Status of commands processed by the TIM App */
	TIM_HkTelemetryPkt.tim_command_count       = 0;
	TIM_HkTelemetryPkt.tim_command_error_count = 0;
	TIM_HkTelemetryPkt.tim_command_image_count = 0;
	TIM_HkTelemetryPkt.tim_command_temps_count = 0;

	//TIM_HkTelemetryPkt.tim_last_pic_loc[0] = '\0';
    memset(TIM_HkTelemetryPkt.tim_last_image_sent, '\0', sizeof(TIM_HkTelemetryPkt.tim_last_image_sent));
	//TIM_HkTelemetryPkt.tim_last_vid_loc[0] = '\0';
    memset(TIM_HkTelemetryPkt.tim_last_temps_sent, '\0', sizeof(TIM_HkTelemetryPkt.tim_last_temps_sent));


	CFE_EVS_SendEvent(TIM_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"TIM: RESET command");
	return;

} /* End of TIM_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  TIM_TakeStill (Generic)                                          */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function takes a picture using raspistill, based on info      */
/*         from the global (CFE_SB_MsgPtr_t *) TIMMsgPtr                    */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void TIM_SendImageFile(void)
{   
	/* Creating a pointer to handle the TIMMsgPtr as TIM_PIC_CMD_PKT  */
	TIM_PIC_CMD_PKT_t *PicCmdPtr;
	PicCmdPtr = (TIM_PIC_CMD_PKT_t *) TIMMsgPtr;
	
	/* virtual_path[] holds the location where ALL pictures will be saved */
	char virtual_path[] = {"/ram/pictures/"};
	char local_path[ OS_MAX_PATH_LEN ];
	
	/*
	** Feedback variable from OS_Translate
	*/
	int32 translate_status = 0;
	/*
	** Feedback variable from OS_QueuePut
	*/
	int32 QueueResult;

	/*
	** MINIMUM_TIME is the hardware time-delay between each picture
	** The camera module may be able to take a picture closer to
	** 400 ms, but I used 500 just to make sure there wouldn't be as
	** many inconsistencies. For a multi-picture sequence, 
	** 500ms should be alotted for each picture
	*/
	int MINIMUM_TIME = 500;
	int sequence_time = MINIMUM_TIME * PicCmdPtr->tim_sequence;
	
	/* Holds all command arguments except output location/file */
	char raspi_call[ TIM_RASPICALL_MAX_LEN ];
	
	/* Holds location/filename */
	char full_path[ OS_MAX_PATH_LEN ];
	
	/* Holds full command to be sent to Cameraman */
	char full_sys_call[ TIM_SYS_CALL_MAX_LEN ];
	
	
	/*
	** Translate the path
	*/
	if ( OS_TranslatePath(virtual_path, (char *)local_path) != OS_FS_SUCCESS )
	{
		translate_status = OS_FS_ERR_PATH_INVALID;
		OS_printf("OS_TranslatePath Status: %d \n", translate_status);
		return;
	}

	/* Concatonate the command, path and filename */
	sprintf(raspi_call,
		 "raspistill -n -tl 500 -t %d -q 25 -h 400 -w 400 -o ", 
		 sequence_time);
	sprintf(full_path,
		"%s%s",
		local_path, PicCmdPtr->PicName);
	sprintf(full_sys_call,
		 "%s%s",
		 raspi_call, full_path);
	
	OS_printf("system call for cameraman:\n%s\n", full_sys_call);
	
	/* Move full_sys_call onto OS_Queue */
	/* the last parameter is currently unused */
	QueueResult = OS_QueuePut( Cameraman_QueueID,
					full_sys_call,
					TIM_SYS_CALL_MAX_LEN,
					0 );
	if ( QueueResult != OS_SUCCESS )
	{
		char *QueueTaskText = "Cameraman Queue Put";
		CFE_EVS_SendEvent(TIM_QUEUE_PUT_ERR_EID, CFE_EVS_ERROR,
		"%s error: OS_QueuePut failed: result = %d",
		QueueTaskText, QueueResult);
	}

	/* */
	
	/* Remove before flight */
	/* system(full_sys_call); */

	return;
} /* End of TIM_TakeStill() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TIM_TakeVideo() -- Take a video for a specified amount of time  */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void TIM_SendTempsFile(void)
{
	/* Creating a pointer to handle the TIMMsgPtr as TIM_VID_CMD_PKT  */
	TIM_VID_CMD_PKT_t *VidCmdPtr;
	VidCmdPtr = (TIM_VID_CMD_PKT_t *) TIMMsgPtr;

	/* virtual_path[] holds the location where ALL videos will be saved */
	char virtual_path[] = {"/ram/videos/"};
	char local_path[ OS_MAX_PATH_LEN ];
	
	/*
	** Feedback variable from OS_Translate
	*/
	int32 translate_status = 0;
	/*
	** Feedback variable from OS_QueuePut
	*/
	int32 QueueResult;
	
	/* Holds all command arguments except output location/file */
	char raspi_call[ TIM_RASPICALL_MAX_LEN ];
	
	/* Holds location/filename */
	char full_path[ OS_MAX_PATH_LEN ];
	
	/* Holds full command to be sent to Cameraman */
	char full_sys_call[ TIM_SYS_CALL_MAX_LEN ];
	
	/*
	** Translate the path
	*/
	if ( OS_TranslatePath(virtual_path, (char *)local_path) != OS_FS_SUCCESS )
	{
		translate_status = OS_FS_ERR_PATH_INVALID;
		OS_printf("OS_TranslatePath Status: %d \n", translate_status);
		return;
	}
	
	
	/* Concatonate the command, path and filename */
	sprintf(raspi_call,
		 "raspivid -n -t %d -h 400 -w 400 -o ",
		 VidCmdPtr->VidLength);
	sprintf(full_path,
		 "%s%s",
		 local_path, VidCmdPtr->VidName);
	sprintf(full_sys_call,
		 "%s%s",
		 raspi_call, full_path);

	OS_printf("system call shown below:\n%s\n", full_sys_call);

	/* Move full_sys_call onto OS_Queue */
	/* the last parameter is currently unused */
	QueueResult = OS_QueuePut( Cameraman_QueueID,
					full_sys_call,
					TIM_SYS_CALL_MAX_LEN,
					0 );
	if ( QueueResult != OS_SUCCESS )
	{
		char *QueueTaskText = "Cameraman Queue Put";
		CFE_EVS_SendEvent(TIM_QUEUE_PUT_ERR_EID, CFE_EVS_ERROR,
		"%s error: OS_QueuePut failed: result = %d",
		QueueTaskText, QueueResult);
	}

	/* */
	
	/* Remove before flight */
	/* system(full_sys_call); */

	return;
} /* End of TIM_TakeVideo() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* TIM_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean TIM_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{     
    boolean result = TRUE;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(TIM_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        TIM_HkTelemetryPkt.tim_command_error_count++;
    }

    return(result);

} /* End of TIM_VerifyCmdLength() */

