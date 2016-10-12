/** @file tim_app_msg.h
 *  Defines TIM app messages and info
 */
#ifndef _tim_app_msg_h_
#define _tim_app_msg_h_

// TIM App command codes

/** No operation (NOOP) command */
#define TIM_APP_NOOP_CC 0
/** Reset program's counters and saved file locations */
#define TIM_APP_RESET_COUNTERS_CC 1
/** Message to tell the program an error occurred */
#define TIM_APP_ERROR_CC 2
/** Tells the program to take an image from camera 0 */
#define TIM_APP_SEND_IMAGE0_CC 3
/** Tells the program to take an  image from camera 1 */
#define TIM_APP_SEND_IMAGE1_CC 5
/** Tells the program to send the temperature data */
#define TIM_APP_SEND_TEMPS_CC 4

/** A command type with no arguments. Contains a header array. */
typedef struct { uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE]; } TIM_NoArgsCmd_t;

/** A command type containing an image name and a header array */
typedef struct {
  uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE];
  char ImageName[TIM_MAX_IMAGE_NAME_LEN];
} TIM_IMAGE_CMD_PKT_t;

/** A command type containing a temperature file name and a header array */
typedef struct {
  uint8 CmdHeader[CFE_SB_CMD_HDR_SIZE];
  char TempsName[TIM_MAX_TEMPS_NAME_LEN];
} TIM_TEMPS_CMD_PKT_t;

/** Defines the structure of a housekeeping packet. */
typedef struct {
  /** The packet header */
  uint8 TlmHeader[CFE_SB_TLM_HDR_SIZE];
  /** The amount of commands processed by the TIM app */
  uint8 tim_command_count;
  /** The amount of TIM commands that have caused errors */
  uint8 tim_command_error_count;
  /** The amount of images sent by the TIM */
  uint8 tim_command_image_count;
  /** The amount of temperature files sent by the TIM */
  uint8 tim_command_temps_count;
  /** The name of the last image file sent */
  char tim_last_image_sent[OS_MAX_PATH_LEN];
  /** The name of the last temperature file sent */
  char tim_last_temps_sent[OS_MAX_PATH_LEN];
  /** Indicator of the last sent data. 0 - nothing sent, 1 - image, 2 - temperatures, 3 - busy? */
  uint8 tim_last_sent;

} OS_PACK tim_hk_tlm_t;

/** Size of tim_hk_tlm_t variables */
#define TIM_APP_HK_TLM_LNGTH sizeof(tim_hk_tlm_t)

#endif