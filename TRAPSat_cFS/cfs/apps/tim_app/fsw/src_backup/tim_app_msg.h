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

/*
** TIM_PIC_CMD_PKT_t
** Type definition (send picture with file name)
**
**  uint8 CmdHeader[] standard buffer for ccsds cmd pkt
**
**  char Picture File Name (from '/ram/images/' ) 
**
**  tim_sequence can be removed 
**
**  uint8 spare[3] can be removed (it was a overfill buf)
*/
typedef struct
{
    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char	           PicName[32];
    uint8              tim_sequence;
    uint8              spare[3];
} TIM_PIC_CMD_PKT_t;


/***************************************************************************/

/*
 Type definition (TIM_TakeVideo command pkt - take video with file name)

 char spare is used as a pillow for overrunning text in the filename
	this should be handled by the python truncation, 
	but is added as an extra precaution

*/

typedef struct
{
    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char	       VidName[32];
    char               spare[2];
    uint16             VidLength;
} TIM_VID_CMD_PKT_t;

/*************************************************************************/
/*
** Type definition (TIM App housekeeping)
*/
/* tim_last_image_sent[OS_MAX_PATH_LEN] < 32 ? */
/* tim_last_temps_sent[OS_MAX_PATH_LEN] == 22 chars */
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              tim_command_count;
    uint8              tim_command_error_count;
    char               spare[2];
    uint8              tim_command_image_count;
    uint8              tim_command_temps_count;
    char               tim_last_image_sent[OS_MAX_PATH_LEN]; 
    char               tim_last_temps_sent[OS_MAX_PATH_LEN]; 
    TIM_Cameraman_t  CameraInfo;

}   OS_PACK tim_hk_tlm_t  ;

#define TIM_APP_HK_TLM_LNGTH   sizeof ( tim_hk_tlm_t )

#endif /* _tim_app_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
