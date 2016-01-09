The Software Bus unit test results are expected to have the
following results for each of the SB services source files:

ut_cfe_sb PASSED 146 tests.
ut_cfe_sb FAILED 0 tests.

gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/sb/cfe_sb_api.c' 98.70%  537
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/sb/cfe_sb_buf.c' 100.00%  51
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/sb/cfe_sb_init.c' 100.00%  45
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/sb/cfe_sb_priv.c' 100.00%  136
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/sb/cfe_sb_task.c' 99.45%  361
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/sb/cfe_sb_util.c' 100.00%  86

==========================================================================

cfe_sb_api.c - 98.70% coverage -

In CFE_SB_SendMsgFull
        -: 1154:    /* validate the msgid in the message */
       26: 1155:    if(CFE_SB_ValidateMsgId(MsgId) != CFE_SUCCESS){
        1: 1156:        CFE_SB_LockSharedData(__func__,__LINE__);
        1: 1157:        CFE_SB.HKTlmMsg.MsgSendErrCnt++;
        1: 1158:        if (CopyMode == CFE_SB_SEND_ZEROCOPY)
        -: 1159:        {
    #####: 1160:            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId,
MsgPtr);
    #####: 1161:            CFE_SB_DecrBufUseCnt(BufDscPtr);
        -: 1162:        }
        1: 1163:        CFE_SB_UnlockSharedData(__func__,__LINE__);
        1: 1164:
CFE_EVS_SendEventWithAppID(CFE_SB_SEND_INV_MSGID_EID,CFE_EVS_ERROR,CFE_SB.AppId,
        -: 1165:            "Send Err:Invalid MsgId(0x%x)in msg,App %s",
        -: 1166:            MsgId,CFE_SB_GetAppTskName(TskId,FullName));
        1: 1167:        return CFE_SB_BAD_ARGUMENT;
        -: 1168:    }/* end if */

        -: 1172:    /* Verify the size of the pkt is < or = the mission
defined max */
       25: 1173:    if(TotalMsgSize > CFE_SB_MAX_SB_MSG_SIZE){
        1: 1174:        CFE_SB_LockSharedData(__func__,__LINE__);
        1: 1175:        CFE_SB.HKTlmMsg.MsgSendErrCnt++;
        1: 1176:        if (CopyMode == CFE_SB_SEND_ZEROCOPY)
        -: 1177:        {
    #####: 1178:            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId,
MsgPtr);
    #####: 1179:            CFE_SB_DecrBufUseCnt(BufDscPtr);
        -: 1180:        }
        1: 1181:        CFE_SB_UnlockSharedData(__func__,__LINE__);
        1: 1182:
CFE_EVS_SendEventWithAppID(CFE_SB_MSG_TOO_BIG_EID,CFE_EVS_ERROR,CFE_SB.AppId,
        -: 1183:            "Send Err:Msg Too Big
MsgId=0x%x,app=%s,size=%d,MaxSz=%d",
        -: 1184:
MsgId,CFE_SB_GetAppTskName(TskId,FullName),TotalMsgSize,CFE_SB_MAX_SB_MSG_SIZE);
        1: 1185:        return CFE_SB_MSG_TOO_BIG;
        -: 1186:    }/* end if */

       24: 1195:    if(RtgTblIdx == CFE_SB_AVAILABLE){
        -: 1196:
        9: 1197:        CFE_SB.HKTlmMsg.NoSubscribersCnt++;
        -: 1198:
        9: 1199:        if (CopyMode == CFE_SB_SEND_ZEROCOPY){
    #####: 1200:            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId,
MsgPtr);
    #####: 1201:            CFE_SB_DecrBufUseCnt(BufDscPtr);
        -: 1202:        }
        -: 1203:
        9: 1204:        CFE_SB_UnlockSharedData(__func__,__LINE__);
        -: 1205:
        -: 1206:        /* Determine if event can be sent without causing
recursive event problem */
        9: 1207:
if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_SEND_NO_SUBS_EID_BIT) ==
CFE_SB_GRANTED){
        -: 1208:
        9: 1209:
CFE_EVS_SendEventWithAppID(CFE_SB_SEND_NO_SUBS_EID,CFE_EVS_INFORMATION,CFE_SB.AppId,
        -: 1210:              "No subscribers for MsgId 0x%x,sender %s",
        -: 1211:              MsgId,CFE_SB_GetAppTskName(TskId,FullName));
        -: 1212:
        -: 1213:           /* clear the bit so the task may send this event
again */
        9: 1214:
CFE_CLR(CFE_SB.StopRecurseFlags[TskId],CFE_SB_SEND_NO_SUBS_EID_BIT);
        -: 1215:        }/* end if */
        -: 1216:
        9: 1217:        return CFE_SUCCESS;
        -: 1218:    }/* end if */

        -: 1272:    /* Send the packet to all destinations  */
       28: 1273:    for (i=0; i < RtgTblPtr -> Destinations; i++) {
        -: 1274:
        -: 1275:        /* The DestPtr should never be NULL in this loop, this
is just extra
        -: 1276:           protection in case of the unforseen */
       14: 1277:        if(DestPtr == NULL){
    #####: 1278:          break;
        -: 1279:        }
        
REASON: None - A DCR was entered to get full coverage of this file.

==========================================================================

cfe_sb_buf.c - 100% coverage

==========================================================================

cfe_sb_init.c - 100% coverage

==========================================================================

cfe_sb_priv.c - 100% coverage

==========================================================================

cfe_sb_task.c - 99.45% coverage 

The first line of code that is not executed in Unit Testing is in the
CFE_SB_TaskMain function.  The code snippet is as follows:

        -:  145:    /* Main loop */
        5:  146:    while (Status == CFE_SUCCESS)
        -:  147:    {
        -:  148:        /* Increment the Main task Execution Counter */
        1:  149:        CFE_ES_IncrementTaskCounter();
        -:  150:
        1:  151:        CFE_ES_PerfLogExit(CFE_SB_MAIN_PERF_ID);
        -:  152:
        -:  153:        /* Pend on receipt of packet */
        1:  154:        Status = CFE_SB_RcvMsg(&CFE_SB.CmdPipePktPtr,
        -:  155:                                CFE_SB.CmdPipe,
        -:  156:                                CFE_SB_PEND_FOREVER);
        -:  157:
        1:  158:        CFE_ES_PerfLogEntry(CFE_SB_MAIN_PERF_ID);
        -:  159:
        1:  160:        if(Status == CFE_SUCCESS)
        -:  161:        {
        -:  162:            /* Process cmd pipe msg */
    #####:  163:            CFE_SB_ProcessCmdPipePkt();
        -:  164:        }else{
        1:  165:            CFE_ES_WriteToSysLog("SB:Error reading cmd
pipe,RC=0x%08X\n",Status);
        -:  166:        }/* end if */
        -:  167:
        -:  168:    }/* end while */
        -:  169:
        -:  170:    /* while loop exits only if CFE_SB_RcvMsg returns error */
        2:  171:    CFE_ES_ExitApp(CFE_ES_CORE_APP_RUNTIME_ERROR);
        -:  172:
        2:  173:}/* end CFE_SB_TaskMain */

REASON: Have not found a way to execute this line while allowing the unit test
        terminate normally. 

The next line of code cannot be executed is found in CFE_SB_AppInit:

        -:  257:    /* Be sure the number of events to register for filtering
        -:  258:    ** does not exceed CFE_EVS_MAX_EVENT_FILTERS */
       12:  259:    if(CFE_EVS_MAX_EVENT_FILTERS < CfgFileEventsToFilter){
    #####:  260:      CfgFileEventsToFilter = CFE_EVS_MAX_EVENT_FILTERS;
        -:  261:    }/* end if */
        
REASON: Cannot be executed without altering EVS or SB cfg parameters.

==========================================================================

cfe_sb_util.c - 100% coverage

==========================================================================

