/** @file tim_app_msgids.h
 *  Defines TIM app message IDs. These IDs are used in tim_app.c to direct
 *  commands to the appropriate pipes for the CFE.
*/
#ifndef _tim_app_msgids_h_
#define _tim_app_msgids_h_

/*
** not in this version of the cFS
#include "mission_msgids.h"
#include "platform_base_msgids.h"
*/

// Message IDs for commands. Used to direct commands to pipes in CFE
/** Indicates a command message */
#define TIM_APP_CMD_MID 0x188A /* CFE_CMD_MID_BASE + TIM_APP_CMD_MID_OFFSET */
/** Indicates a send housekeeping command message */
#define TIM_APP_SEND_HK_MID                                                    \
  0x188B /* CFE_CMD_MID_BASE + TIM_APP_SEND_HK_MID_OFFSET */
/** Indicates a housekeeping telemetry message */
#define TIM_APP_HK_TLM_MID                                                     \
  0x088B /* CFE_TLM_MID_BASE + TIM_APP_HK_TLM_MID_OFFSET */

#endif