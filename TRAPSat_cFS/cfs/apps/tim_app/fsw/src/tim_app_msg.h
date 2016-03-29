/*******************************************************************************
** File:
**   tim_app_msg.h 
**
** Purpose: 
**  Define TIM App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _tim_app_msg_h_
#define _tim_app_msg_h_

/*
** TIM App command codes
*/
#define TIM_APP_NOOP_CC                 0
#define TIM_APP_RESET_COUNTERS_CC       1
#define TIM_APP_ERROR_CC                2 /* get rid of this one? */
#define TIM_APP_SEND_IMAGE_CC           3 /* send picture file */
#define TIM_APP_SEND_TEMPS_CC           4 /* send temperature file */


/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} TIM_NoArgsCmd_t;


/***************************************************************************/

typedef struct
{
    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char	           ImageName[TIM_MAX_IMAGE_NAME_LEN];
} TIM_IMAGE_CMD_PKT_t;


/***************************************************************************/

typedef struct 
{
    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char	           TempsName[TIM_MAX_TEMPS_NAME_LEN];
} TIM_TEMPS_CMD_PKT_t;

/*************************************************************************/
/*
** Type definition (TIM App housekeeping)
*/
/* tim_last_image_sent[OS_MAX_PATH_LEN] < 32 ? */
/* tim_last_temps_sent[OS_MAX_PATH_LEN] == 22 chars */
/* 
** tim_last_sent =
** 0 if nothing sent
** 1 if last sent was image
** 2 if last sent was temps
** 3 if busy?
*/
typedef struct 
{
    uint8               TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8               tim_command_count;
    uint8               tim_command_error_count;
    uint8               tim_command_image_count;
    uint8               tim_command_temps_count;
    char                tim_last_image_sent[OS_MAX_PATH_LEN]; 
    char                tim_last_temps_sent[OS_MAX_PATH_LEN];
    uint8               tim_last_sent;

}   OS_PACK tim_hk_tlm_t  ;

#define TIM_APP_HK_TLM_LNGTH   sizeof ( tim_hk_tlm_t )

#endif /* _tim_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
