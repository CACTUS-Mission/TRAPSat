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

/*************************************************************************/
/*
** Definitions redundantly copied from TIM
** quick and dirty method of sharing this info
*/
#define VC0706_IMAGE_CMD_MID           0x188A /* This should be == TIM_APP_CMD_MID */
#define VC0706_IMAGE0_CMD_CODE          3      /* This should be == TIM_APP_SEND_IMAGE0_CC */
#define VC0706_IMAGE1_CMD_CODE          5      /* This should be == TIM_APP_SEND_IMAGE1_CC */
#define VC0706_MAX_IMAGE_NAME_LEN      15     /* This should be == TIM_MAX_IMAGE_NAME_LEN */

typedef struct 
{
    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char               ImageName[VC0706_MAX_IMAGE_NAME_LEN];
} VC0706_IMAGE_CMD_PKT_t;

#define VC0706_IMAGE_CMD_LNGTH   sizeof ( VC0706_IMAGE_CMD_PKT_t )

/*************************************************************************/


#endif /* _vc0706_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
