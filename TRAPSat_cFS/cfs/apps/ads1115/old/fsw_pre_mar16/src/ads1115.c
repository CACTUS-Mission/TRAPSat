/*******************************************************************************
** File: ads1115.c
**
** Purpose:
**   This file contains the source code for the ADS1115 App.
**
*******************************************************************************/

/*
** Master Header
*/
#include "ads1115.h"
#include "ads1115_perfids.h"
#include "ads1115_msgids.h"
#include "ads1115_msg.h"
#include "ads1115_events.h"
#include "ads1115_version.h"
#include "ads1115_child.h"

/*
** global data structures
*/
ads1115_hk_tlm_t   ADS1115_HkTelemetryPkt;
CFE_SB_PipeId_t    ADS1115_CommandPipe;
CFE_SB_MsgPtr_t    ADS1115_MsgPtr;

ADS1115_Ch_Data_t  ADS1115_ChannelData;
uint32             ADS1115_ADC_ChildTaskID;


static CFE_EVS_BinFilter_t  ADS1115_EventFilters[] =
       {  /* Event ID    mask */
          {ADS1115_STARTUP_INF_EID,       0x0000},
          {ADS1115_COMMAND_ERR_EID,       0x0000},
          {ADS1115_COMMANDNOP_INF_EID,    0x0000},
          {ADS1115_COMMANDRST_INF_EID,    0x0000},
       };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* ADS1115_AppMain() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void ADS1115_AppMain( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(ADS1115_PERF_ID);

    ADS1115_AppInit();

    /*
    ** ADS1115 Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(ADS1115_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&ADS1115_MsgPtr, ADS1115_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(ADS1115_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            ADS1115_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of ADS1115_AppMain() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* ADS1115_AppInit() --  initialization                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void ADS1115_AppInit(void)
{
    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register(ADS1115_EventFilters,
                     sizeof(ADS1115_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Create the Software Bus command pipe and subscribe to housekeeping
    **  messages
    */
    CFE_SB_CreatePipe(&ADS1115_CommandPipe, ADS1115_PIPE_DEPTH,"ADS1115_CMD_PIPE");
    CFE_SB_Subscribe(ADS1115_CMD_MID, ADS1115_CommandPipe);
    CFE_SB_Subscribe(ADS1115_SEND_HK_MID, ADS1115_CommandPipe);

    ADS1115_ResetCounters();

    ADS1115_ChildInit();

    CFE_SB_InitMsg(&ADS1115_HkTelemetryPkt,
                   ADS1115_HK_TLM_MID,
                   ADS1115_HK_TLM_LNGTH, TRUE);

    CFE_EVS_SendEvent (ADS1115_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "ADS1115 App Initialized. Version %d.%d.%d.%d",
                ADS1115_MAJOR_VERSION,
                ADS1115_MINOR_VERSION, 
                ADS1115_REVISION, 
                ADS1115_MISSION_REV);
				
} /* End of ADS1115_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  ADS1115_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the ADS1115    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void ADS1115_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(ADS1115_MsgPtr);

    switch (MsgId)
    {
        case ADS1115_CMD_MID:
            ADS1115_ProcessGroundCommand();
            break;

        case ADS1115_SEND_HK_MID:
            ADS1115_ReportHousekeeping();
            break;

        default:
            ADS1115_HkTelemetryPkt.ads1115_command_error_count++;
            CFE_EVS_SendEvent(ADS1115_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"ADS1115: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End ADS1115_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* ADS1115_ProcessGroundCommand() -- ADS1115 ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void ADS1115_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(ADS1115_MsgPtr);

    /* Process "known" ADS1115 app ground commands */
    switch (CommandCode)
    {
        case ADS1115_NOOP_CC:
            ADS1115_HkTelemetryPkt.ads1115_command_count++;
            CFE_EVS_SendEvent(ADS1115_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"ADS1115: NOOP command");
            break;

        case ADS1115_RESET_COUNTERS_CC:
            ADS1115_ResetCounters();
            break;

        /* default case already found during FC vs length test */
        default:
            break;
    }
    return;

} /* End of ADS1115_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  ADS1115_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void ADS1115_ReportHousekeeping(void)
{
    /*
    ** Copy Application Data Structures
    ** to Housekeeping Data Packet
    */
    memcpy((char *) &ADS1115_HkTelemetryPkt.ads1115_ch_data, 
        (char *) &ADS1115_ChannelData, sizeof(ADS1115_Ch_Data_t));

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &ADS1115_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &ADS1115_HkTelemetryPkt);

    return;

} /* End of ADS1115_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  ADS1115_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void ADS1115_ResetCounters(void)
{
    /* Status of commands processed by the ADS1115 App */
    ADS1115_HkTelemetryPkt.ads1115_command_count       = 0;
    ADS1115_HkTelemetryPkt.ads1115_command_error_count = 0;

    /*
    ** Should we clear scientific data, or let it sit on the buffers? 
    ** ADS1115_HkTelemetryPkt.ads1115_ch_data.adc_ch_0 = {0x00, 0x00};
    */
    memset(&ADS1115_HkTelemetryPkt.ads1115_ch_data, 0, sizeof(ADS1115_Ch_Data_t));
    /*
    ADS1115_HkTelemetryPkt.ads1115_ch_data.adc_ch_0 = {0x00, 0x00};
    ADS1115_HkTelemetryPkt.ads1115_ch_data.adc_ch_1 = {0x00, 0x00};
    ADS1115_HkTelemetryPkt.ads1115_ch_data.adc_ch_2 = {0x00, 0x00};
    ADS1115_HkTelemetryPkt.ads1115_ch_data.adc_ch_3 = {0x00, 0x00};
    */
    CFE_EVS_SendEvent(ADS1115_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"ADS1115: RESET command");
    return;

} /* End of ADS1115_ResetCounters() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* ADS1115_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean ADS1115_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
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

        CFE_EVS_SendEvent(ADS1115_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        ADS1115_HkTelemetryPkt.ads1115_command_error_count++;
    }

    return(result);

} /* End of ADS1115_VerifyCmdLength() */

