/** @file tim_app_events.h
 *  Defines TIM app event IDs for informing CFE what the TIM is doing
 */
#ifndef _tim_app_events_h_
#define _tim_app_events_h_

/** This is reserved, currently unused */
#define TIM_RESERVED_EID            0
/** Event indicating the app has started */
#define TIM_STARTUP_INF_EID         1
/** Event indicating there was a command error */
#define TIM_COMMAND_ERR_EID         2
/** Event indicating a No Operation command was received */
#define TIM_COMMANDNOP_INF_EID      3
/** Event indicating a RESET command was received */
#define TIM_COMMANDRST_INF_EID      4
/** Event indicating an invalid message was received */
#define TIM_INVALID_MSGID_ERR_EID   5
/** Event indicating the command received was not the expected length */
#define TIM_LEN_ERR_EID             6

/** ID indicating a message pertaining to camera imaging */
#define TIM_COMMAND_IMAGE_EID       7
/** ID indicating a message pertaining to temperature logging */
#define TIM_COMMAND_TEMPS_EID       8
/** ID indicating a message pertaining to system logging */
#define TIM_COMMAND_LOGS_EID        9

/** Event relating to parallel power flag */
#define TIM_PARALLEL_PWR_EID        10
/** Event relating to parallel TE-1 signal */
#define TIM_PARALLEL_TE_EID         11

/*
#define TIM_CHILD_INIT_EID            9
#define TIM_CHILD_INIT_ERR_EID       10
#define TIM_QUEUE_CREATE_ERR_EID     11
#define TIM_QUEUE_DELETE_ERR_EID     12
#define TIM_QUEUE_PUT_ERR_EID        13
#define TIM_QUEUE_GET_ERR_EID        14
#define TIM_CHILD_UNKWN_ERR_EID		 15
*/

#endif