/************************************************************************
** File:
**    ads1115_events.h 
**
** Purpose: 
**  Define ADS1115 App Events IDs
**
** Notes:
**
**
*************************************************************************/
#ifndef _ads1115_events_h_
#define _ads1115_events_h_

/*
** Main Task Event IDs
*/
#define ADS1115_RESERVED_EID              0
#define ADS1115_STARTUP_INF_EID           1 
#define ADS1115_COMMAND_ERR_EID           2
#define ADS1115_COMMANDNOP_INF_EID        3 
#define ADS1115_COMMANDRST_INF_EID        4
#define ADS1115_INVALID_MSGID_ERR_EID     5 
#define ADS1115_LEN_ERR_EID               6
#define ADS1115_CMD_SET_CH_ST_INF_EID     9
#define ADS1115_CMD_SET_CH_ST_ERR_EID    10

/*
** Child Task Event IDs
*/
#define ADS1115_CHILD_INIT_ERR_EID        7 
#define ADS1115_CHILD_INIT_EID            8
#define ADS1115_CHILD_ADC_ERR_EID        11

#endif /* _ads1115_events_h_ */

/************************/
/*  End of File Comment */
/************************/
