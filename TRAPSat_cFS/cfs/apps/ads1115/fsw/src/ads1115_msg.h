/*******************************************************************************
** File:
**   ads1115_msg.h 
**
** Purpose: 
**  Define ADS1115 App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _ads1115_msg_h_
#define _ads1115_msg_h_

/*
** ADS1115 App command codes
*/
#define ADS1115_NOOP_CC                 0
#define ADS1115_RESET_COUNTERS_CC       1
#define ADS1115_SET_DELAY_CC            2

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} ADS1115_NoArgsCmd_t;


/*
** Type definition ("Change Delay" command)
**
** uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
**  Command Header using CCSDS standard
**
** uint8    childloop_state;
**  
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint8    childloop_state;
} ADS1115_SetDelayCmd_t;

/*************************************************************************/
/*
** Type definition (ADS1115 App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              ads1115_command_error_count;
    uint8              ads1115_command_count;
    ADS1115_Ch_Data_t  ads1115_ch_data;
    uint8              ads1115_childloop_state;
}   OS_PACK ads1115_hk_tlm_t  ;

#define ADS1115_HK_TLM_LNGTH   sizeof ( ads1115_hk_tlm_t )

#endif /* _ads1115_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
