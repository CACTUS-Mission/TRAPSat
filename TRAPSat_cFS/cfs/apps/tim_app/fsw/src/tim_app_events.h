/************************************************************************
** File:
**    tim_app_events.h 
**
** Purpose: 
**  Define TIM App Events IDs
**
** Notes:
**
**
*************************************************************************/
#ifndef _tim_app_events_h_
#define _tim_app_events_h_


#define TIM_RESERVED_EID              0
#define TIM_STARTUP_INF_EID           1 
#define TIM_COMMAND_ERR_EID           2
#define TIM_COMMANDNOP_INF_EID        3 
#define TIM_COMMANDRST_INF_EID        4
#define TIM_INVALID_MSGID_ERR_EID     5 
#define TIM_LEN_ERR_EID               6 

#define TIM_COMMAND_IMAGE_EID           7
#define TIM_COMMAND_TEMPS_EID           8

#define TIM_CHILD_INIT_EID            9
#define TIM_CHILD_INIT_ERR_EID       10 
#define TIM_QUEUE_CREATE_ERR_EID     11
#define TIM_QUEUE_DELETE_ERR_EID     12
#define TIM_QUEUE_PUT_ERR_EID        13
#define TIM_QUEUE_GET_ERR_EID        14
#define TIM_CHILD_UNKWN_ERR_EID	15

#endif /* _tim_app_events_h_ */

/************************/
/*  End of File Comment */
/************************/
