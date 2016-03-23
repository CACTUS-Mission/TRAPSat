/*******************************************************************************
** File:
**   vc0706_msg.h
**
** Purpose:
**  Define VC0706 App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _vc0706_msg_h_
#define _vc0706_msg_h_

/*
** VC0706 App command codes
*/
#define VC0706_NOOP_CC                 0
#define VC0706_RESET_COUNTERS_CC       1

/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} VC0706_NoArgsCmd_t;

/*************************************************************************/
/*
** Type definition (VC0706 App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              vc0706_command_error_count;
    uint8              vc0706_command_count;
    char               vc0706_filename[VC0706_MAX_FILENAME_LEN];

}   OS_PACK vc0706_hk_tlm_t  ;

#define VC0706_HK_TLM_LNGTH   sizeof ( vc0706_hk_tlm_t )

#endif /* _vc0706_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
