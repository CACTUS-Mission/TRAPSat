The TBL Services unit test results are expected to have the
following results for each of the TBL services source files:

ut_cfe_tbl PASSED 239 tests.
ut_cfe_tbl FAILED 0 tests.

gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/tbl/cfe_tbl_api.c' 99.01%  505
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/tbl/cfe_tbl_internal.c' 100.00%  433
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/tbl/cfe_tbl_task_cmds.c' 99.77%  434
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/tbl/cfe_tbl_task.c' 100.00%  93


==========================================================================

cfe_tbl_api.c - 99.01% coverage

CFE_TBL_Register

        3:  426:                            Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, TRUE);
        -:  427:
        3:  428:                            if (Status != CFE_SUCCESS)
        -:  429:                            {
        -:  430:                                /* Unable to get a working buffer - this error is not really */
        -:  431:                                /* possible at this point during table registration.  But we */
        -:  432:                                /* do need to handle the error case because if the function */
        -:  433:                                /* call did fail, WorkingBufferPtr would be a NULL pointer. */
    #####:  434:                                CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);
    #####:  435:                                CFE_ES_WriteToSysLog("CFE_TBL:Register-Failed to get work buffer for '%s.%s' (ErrCode=0x%08X)\n",
        -:  436:                                                     AppName, Name, Status);
        -:  437:                            }


CFE_TBL_Update

        4: 1053:    if (Status != CFE_TBL_ERR_BAD_APP_ID)
        -: 1054:    {
        -: 1055:        /* Translate AppID of caller into App Name */
        4: 1056:        CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);
        -: 1057:    }
        -: 1058:
        -: 1059:    /* On Error conditions, notify ground of screw up */
        4: 1060:    if (Status < 0)
        -: 1061:    {
        1: 1062:        if (RegRecPtr != NULL)
        -: 1063:        {
    #####: 1064:            CFE_EVS_SendEventWithAppID(CFE_TBL_UPDATE_ERR_EID,
        -: 1065:                                       CFE_EVS_ERROR,
        -: 1066:                                       CFE_TBL_TaskData.TableTaskAppId,
        -: 1067:                                       "%s Failed to Update '%s', Status=0x%08X",
        -: 1068:                                       AppName, RegRecPtr->Name, Status);
        -: 1069:        }

This event message is never issued in unit testing because in order for it to
occur, the function CFE_TBL_UpdateInternal would be required to return an error
code.  At the current time, the CFE_TBL_UpdateInternal function only returns
either CFE_SUCCESS or informational status. The event message is being
maintained in the event the CFE_TBL_UpdateInternal function is modified to
return an error.

CFE_TBL_Manage

        -: 1428:            /* Validate the specified Table */
       10: 1429:            Status = CFE_TBL_Validate(TblHandle);
        -: 1430:
       10: 1431:            if (Status != CFE_SUCCESS)
        -: 1432:            {
        -: 1433:                /* If an error occurred during Validate, then do not perform any more managing */
    #####: 1434:                FinishedManaging = TRUE;
        -: 1435:            }
        -: 1436:        }

The FinishedManaging variable can currently never be set because the only error
conditions that CFE_TBL_Validate can return are already captured by the
validation performed in the CFE_TBL_GetStatus function.


CFE_TBL_Modified

        2: 1670:        AccessIterator = RegRecPtr->HeadOfAccessList;
        6: 1671:        while (AccessIterator != CFE_TBL_END_OF_LIST)
        -: 1672:        {
        -: 1673:            /* Only notify *OTHER* applications that the contents have changed */
        2: 1674:            if (CFE_TBL_TaskData.Handles[AccessIterator].AppId != ThisAppId)
        -: 1675:            {
    #####: 1676:                CFE_TBL_TaskData.Handles[AccessIterator].Updated = TRUE;
        -: 1677:            }
        -: 1678:
        2: 1679:            AccessIterator = CFE_TBL_TaskData.Handles[AccessIterator].NextLink;
        -: 1680:        }
        -: 1681:    }

==========================================================================

cfe_tbl_internal.c - 100% coverage

==========================================================================

cfe_tbl_task_cmds.c - 99.77% coverage

In CFE_TBL_LoadCmd:

        1:  626:                        else if (Status ==
CFE_TBL_ERR_NO_BUFFER_AVAIL)
        -:  627:                        {
        1:  628:
CFE_EVS_SendEvent(CFE_TBL_NO_WORK_BUFFERS_ERR_EID,
        -:  629:                                              CFE_EVS_ERROR,
        -:  630:                                              "No working
buffers available for table '%s'",
        -:  631:
TblFileHeader.TableName);
        -:  632:                        }
        -:  633:                        else
        -:  634:                        {
    #####:  635:
CFE_EVS_SendEvent(CFE_TBL_INTERNAL_ERROR_ERR_EID,
        -:  636:                                              CFE_EVS_ERROR,
        -:  637:                                              "Internal Error
(Status=0x%08X)",
        -:  638:                                              Status);
        -:  639:                        }

This event message is only issued when an unexpected error is detected in
underlying function calls.  In the current implementation, there isn't any
possibility of an unexpected error to occur.

==========================================================================

cfe_tbl_task.c - 100% coverage

==========================================================================
