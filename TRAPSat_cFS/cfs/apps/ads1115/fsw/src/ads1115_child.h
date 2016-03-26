

#ifndef _ads1115_child_h_
#define _ads1115_child_h_

/*
** ADS1115 Master Header
*/
#include "ads1115.h"

/*
** Platform Required header files
*/
#include <stdio.h>   /* Standard input/output definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
/* Check necessity of each of these to trim fat */

extern ads1115_hk_tlm_t       ADS1115_HkTelemetryPkt;
extern uint32                 ADS1115_ADC_ChildTaskID;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* ADS1115 child task global function prototypes                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**
**  \brief Child Task Initialization Function
**
**  \par Description
**       This function is invoked during ADS1115 application startup initialization to
**       create and initialize the Analog to Digital Converter Child Task.  The purpose for the child task
**       is to read the ADC device's 4 data channels and store the latest data for housekeeping.  
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS     \endcode
**  \retstmt Error return codes from #CFE_ES_CreateChildTask \endcode
**  \endreturns
**
**  \sa #ADS1115_AppInit
**/
int32 ADS1115_ChildInit(void);


/**
**  \brief Child Task Entry Point Function
**
**  \par Description
**       This function is the entry point for the ADS1115 application child task.  The
**       function registers with CFE as a child task, 
**       interface with the parent task and calls the child task main loop function.
**       Should the main loop function return due to a breakdown in the interface
**       handshake with the parent task, this function will self delete as a child
**       task with CFE. There is no return from #CFE_ES_DeleteChildTask.
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**
**  \sa #CFE_ES_DeleteChildTask, #FM_ChildLoop
**/
void ADS1115_ADC_ChildTask(void);


/**
**  \brief Child Task Main Loop Processor Function
**
**  \par Description
**       This function is the main loop for the ADS1115 application child task. 
**
**  \par Assumptions, External Events, and Notes:
**
**  \param [in]  (none)
**
**  \returns
**  \retcode (none) \endcode
**  \endreturns
**/
void ADS1115_ChildLoop(void);


/*
** int ADS1115_ReadADCChannels(void)
**
** Read from all four channels of the ADS1115
** (Analog to Digital Converter (16-bit buffer per channel))
** Assumed to be mounted at /dev/i2c-1
**
** Returns:
**    zero on success
**    negative value on error
*/
int ADS1115_ReadADCChannels(void);

/*
** int ADS1115_StoreADCChannels(void);
**
** Read the ADC channel data struct, store to file
** Returns
**    0 on success.
**    negative on error
*/
int ADS1115_StoreADCChannels(void);


/*
** int ADS1115_SendTimFileName(char *file_name);
** Returns 0 on success
** Returns -1 on error
*/
int ADS1115_SendTimFileName(char *file_name);


#endif /* _ads1115_child_h_ */

/************************/
/*  End of File Comment */
/************************/

