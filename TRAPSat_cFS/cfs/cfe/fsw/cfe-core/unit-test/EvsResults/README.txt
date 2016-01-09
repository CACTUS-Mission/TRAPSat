The EVS Services unit test results are expected to have the
following results for each of the EVS services source files:

ut_cfe_evs PASSED 175 tests.
ut_cfe_evs FAILED 0 tests.

gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/evs/cfe_evs.c'
100.00%  119
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/evs/cfe_evs_log.c'
100.00%  66
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/evs/cfe_evs_task.c'
99.60%  503
gcov: '/home/wmoleski/CFS/mission_2014/cfe/fsw/cfe-core/src/evs/cfe_evs_utils.c'
98.37%  123


==========================================================================

cfe_evs.c - 100.00 % coverage

==========================================================================

cfe_evs_log.c - 100.00% coverage

==========================================================================
cfe_evs_task.c - 99.60% % coverage

The lines not covered are in CFE_EVS_TaskInit:

        -:  401:   /* Register EVS task for event services */
        5:  402:   Status = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
        5:  403:   if (Status != CFE_SUCCESS)
        -:  404:   {
    #####:  405:      CFE_ES_WriteToSysLog("EVS:Call to CFE_EVS_Register
Failed:RC=0x%08X\n",Status);
    #####:  406:      return Status;
        -:  407:   }

REASON: None - A DCR was entered to get full coverage of this file.

==========================================================================
cfe_evs_utils.c - 98.37% coverage

The lines not covered are in EVS_SendEvent:

        -:  634:      /* Were any characters truncated in the buffer? */
      119:  635:      if (ExpandedLength >= CFE_EVS_MAX_MESSAGE_LENGTH)
        -:  636:      {
        -:  637:         /* Mark character before zero terminator to indicate
truncation */
    #####:  638:         EVS_Packet.Message[CFE_EVS_MAX_MESSAGE_LENGTH - 2] =
CFE_EVS_MSG_TRUNCATED;
    #####:  639:         CFE_EVS_GlobalData.EVS_TlmPkt.MessageTruncCounter++;
        -:  640:      }

==========================================================================

REASON: None - A DCR was entered to get full coverage of this file.