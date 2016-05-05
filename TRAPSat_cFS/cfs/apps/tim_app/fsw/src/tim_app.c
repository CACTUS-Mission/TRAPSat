/*******************************************************************************
** File: tim_app.c
**
** Purpose:
**   This file contains the source code for the TIM App.
**
*******************************************************************************/
/*
**   Include Files:
*/

#include "tim_app.h"
#include "tim_app_events.h"
#include "tim_app_msg.h"
#include "tim_app_msgids.h"
#include "tim_app_perfids.h"
#include "tim_app_version.h"
#include "tim_serial_out.h"

// Global data
// TODO: most of this could probably easily be de-globalized.
/** TODO: document this */
tim_hk_tlm_t TIM_HkTelemetryPkt;
/** Pipe ID for TIM commands */
CFE_SB_PipeId_t TIM_CommandPipe;
/** Pointer for TIM messages */
CFE_SB_MsgPtr_t TIMMsgPtr;

// Serial Status Data
/** TODO: document this */
serial_out_t TIM_SerialUSB;
/** Serial busy? (0|1). Replace with bool? Ask Keegan. */
int serial_busy;
/** Type of the last sent serial message */
int serial_last_sent;

/** Maximum wait count for temperature readings */
const int TIM_TEMP_MAX_COUNT = 5;
/** Count of waits since last temperature reading */
int temp_wait_count;
/** Holds last timer event (0|1). Consider bool? Ask Keegan. */
int timer_event_last;

/** Maximum wait count for image captures */
const int TIM_IMAGE_MAX_COUNT = 3;
/** Count of waits since last image */
int image_wait_count;
/** Unused? Ask Keegan */
int image_event_last;

// Globals for poweroff timer
/** ID number for the power-off timer */
uint32 poweroff_timer_id;
/** Resolution in microseconds for the poweroff timer clock.
 *  100,000 microseconds resolution -- should use very little clock time with
 * low resolution
 */
uint32 poweroff_timer_clock_accuracy = 100000;

/*
** global data for child task (cameraman) use
*/
// uint32             	TIM_ChildTaskID;
// uint32			    Cameraman_QueueID;

/** TIM event IDs and their corresponding masks */
static CFE_EVS_BinFilter_t TIM_EventFilters[] = {
    /* Event ID    mask */
    {TIM_STARTUP_INF_EID, 0x0000},
    {TIM_COMMAND_ERR_EID, 0x0000},
    {TIM_COMMANDNOP_INF_EID, 0x0000},
    {TIM_COMMANDRST_INF_EID, 0x0000},
};

/*
** Prototype Section
*/

/** Application entry point and main process loop */
void TIM_AppMain(void) {
  /** Status of command packet receipt */
  int32 status;
  /** CFE run status of this app */
  uint32 RunStatus = CFE_ES_APP_RUN;

  // Log start of this app
  CFE_ES_PerfLogEntry(TIM_APP_PERF_ID);

  // Initialize application
  TIM_AppInit();

  // TIM Runloop
  while (CFE_ES_RunLoop(&RunStatus) == TRUE) {
    // Log app exit (to show performance of initialization/last run)
    CFE_ES_PerfLogExit(TIM_APP_PERF_ID);

    // Pend on receipt of command packet -- timeout set to 500 millisecs
    status = CFE_SB_RcvMsg(&TIMMsgPtr, TIM_CommandPipe, 500);

    // Log app entry to show performance of timer and command processing
    CFE_ES_PerfLogEntry(TIM_APP_PERF_ID);

    // Update timer status
    TIM_UpdateTimerEvent();

    // If command packet receipt is successful, process the packet.
    if (status == CFE_SUCCESS) {
      TIM_ProcessCommandPacket();
    }
  }

  // Exit application
  CFE_ES_ExitApp(RunStatus);
}

/** Initialization of app */
void TIM_AppInit(void) {
  /*
  ** Disable debugging prints
  */
  // OS_printf_disable();

  // Register the app with cFE Executive Services
  CFE_ES_RegisterApp();

  // Register the events
  CFE_EVS_Register(TIM_EventFilters,
                   sizeof(TIM_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                   CFE_EVS_BINARY_FILTER);

  // Create the Software Bus command pipe and subscribe to housekeeping messages
  CFE_SB_CreatePipe(&TIM_CommandPipe, TIM_PIPE_DEPTH, "TIM_CMD_PIPE");
  // Subscribe the command pipe to the command messages
  CFE_SB_Subscribe(TIM_APP_CMD_MID, TIM_CommandPipe);
  // Subscribe the command pipe to the housekeeping messages
  CFE_SB_Subscribe(TIM_APP_SEND_HK_MID, TIM_CommandPipe);

  TIM_ResetCounters();

  // TODO: move these assignments to initialization up top? There's not much
  // reason for them to start uninitialized if this is the only path through the
  // program.
  serial_busy = 0;
  serial_last_sent = 0;

  temp_wait_count = 0;
  timer_event_last = 0;

  image_wait_count = 0;
  image_event_last = 0;

  // Check to make sure serial will connect
  if (serial_out_init(&TIM_SerialUSB, (char *)SERIAL_OUT_PORT) < 0) {
    OS_printf("TIM: error, serial out init failed.\n");
  }

  // serial_out_init(&TIM_SerialUSB, SERIAL_OUT_PORT);

  // Initialize parallel communications
  TIM_Parallel_Init();

  // OS_printf("TIM_APP: Attempting to initialize reboot timer.\n");

  // Create the power-off timer attached to timer_callback_poweroff_system()
  int32 timer_ret = OS_TimerCreate(
      (uint32 *)&poweroff_timer_id, (const char *)"poweroff_timer",
      (uint32 *)&poweroff_timer_clock_accuracy,
      (OS_TimerCallback_t)&timer_callback_poweroff_system);

  // Check for timer creation success
  if (timer_ret != OS_SUCCESS) {
    OS_printf("OS_TimerCreate() failed to attach timer to "
              "timer_callback_poweroff_system() function! returned: %d\n",
              timer_ret);
  }

  // 10 seconds in microseconds -- function should never be called twice, but
  // this is a failsafe of sorts.
  uint32 poweroff_interval = 10000000;
  OS_printf("TIM_APP: Attempting to set reboot timer to %d microseconds.\n",
            POWEROFF_TIME);
  // Set the power-off timer
  timer_ret = OS_TimerSet((uint32)poweroff_timer_id, (uint32)POWEROFF_TIME,
                          (uint32)poweroff_interval);
  // Check for timer set success
  if (timer_ret != OS_SUCCESS) {
    OS_printf("OS_TimerSet() failed to set start time for "
              "timer_callback_poweroff_system() function! returned: %d\n",
              timer_ret);
  }
  // TIM_ChildInit();

  // Initialize a housekeeping telemetry message
  CFE_SB_InitMsg(&TIM_HkTelemetryPkt, TIM_APP_HK_TLM_MID, TIM_APP_HK_TLM_LNGTH,
                 TRUE);

  // Send an event that indicates app is initialized
  CFE_EVS_SendEvent(TIM_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                    "TIM App Initialized. Version %d.%d.%d.%d",
                    TIM_APP_MAJOR_VERSION, TIM_APP_MINOR_VERSION,
                    TIM_APP_REVISION, TIM_APP_MISSION_REV);
}

/** Processes any packet that is received on the TIM command pipe */
void TIM_ProcessCommandPacket(void) {
  /** Message ID of the command packet */
  CFE_SB_MsgId_t MsgId;

  // Get ID from message pointer
  MsgId = CFE_SB_GetMsgId(TIMMsgPtr);

  switch (MsgId) {
  // Process ground command
  case TIM_APP_CMD_MID:
    TIM_ProcessGroundCommand();
    break;

  // Process housekeeping command
  case TIM_APP_SEND_HK_MID:
    TIM_ReportHousekeeping();
    break;

  // Log invalid command
  default:
    TIM_HkTelemetryPkt.tim_command_error_count++;
    CFE_EVS_SendEvent(TIM_COMMAND_ERR_EID, CFE_EVS_ERROR,
                      "TIM: invalid command packet,MID = 0x%x", MsgId);
    break;
  }
}

/** Process a received ground command */
void TIM_ProcessGroundCommand(void) {
  /** Command code from message */
  uint16 CommandCode;

  // Get command code from message pointer
  CommandCode = CFE_SB_GetCmdCode(TIMMsgPtr);

  // Process "known" TIM app ground commands
  switch (CommandCode) {
  // Received a no operation command, log and do nothing
  case TIM_APP_NOOP_CC:
    TIM_HkTelemetryPkt.tim_command_count++;
    CFE_EVS_SendEvent(TIM_COMMANDNOP_INF_EID, CFE_EVS_INFORMATION,
                      "TIM: NOOP command");
    break;

  // Process reset counter command
  case TIM_APP_RESET_COUNTERS_CC:
    TIM_ResetCounters();
    break;

  // Received send image command from camera 0
  // TODO: generalize this code and replace this case and the next with function
  // calls.
  case TIM_APP_SEND_IMAGE0_CC:
    // If last sent was a temperature file command and serial isn't busy, send
    // image
    if (serial_last_sent == TIM_APP_SEND_TEMPS_CC) {
      if (serial_busy == 0) {
        // Set serial as busy, set last sent serial signal as this one
        serial_busy = 1;
        serial_last_sent = TIM_APP_SEND_IMAGE0_CC;

        // Send the image file
        TIM_SendImageFile();

        // After completion, set serial to ready
        serial_busy = 0;

        // Reset Wait Count when we know its been sent
        image_wait_count = 0;

        // Increment related command counts on telemetry packet
        TIM_HkTelemetryPkt.tim_command_count++;
        TIM_HkTelemetryPkt.tim_command_image_count++;

        // Send event to CFE indicating file was sent
        CFE_EVS_SendEvent(TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
                          "TIM: Send Camera 0 File Command Received and Sent");
      } else {
        // It was time to send a picture but serial was busy
        CFE_EVS_SendEvent(
            TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
            "TIM: Send Camera 0 File Command Received but Serial Busy");
      }
    } else {
      // Increment wait count since last image
      image_wait_count++;

      // If we've waited the max amount since last image, try to send an image
      // regardless of the last command not being a temperature file command
      // TODO: move the code within these conditionals into a function because
      // it's identical to the block above.
      if (image_wait_count >= TIM_IMAGE_MAX_COUNT) {
        if (serial_busy == 0) {
          serial_busy = 1;
          serial_last_sent = TIM_APP_SEND_IMAGE0_CC;
          TIM_SendImageFile();
          serial_busy = 0;

          // Reset Wait Count when we know its been sent
          image_wait_count = 0;

          // Increment related command counts on telemetry packet
          TIM_HkTelemetryPkt.tim_command_count++;
          TIM_HkTelemetryPkt.tim_command_image_count++;

          // Send event to CFE indicating file was sent
          CFE_EVS_SendEvent(
              TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
              "TIM: Send Camera 0 File Command Received and Sent");
        } else {
          // Tried to send an image but the serial was busy
          CFE_EVS_SendEvent(TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
                            "TIM: Send Camera 0 File Command Received but "
                            "Serial Busy, WC = [%d]",
                            image_wait_count);
        }

      } else {
        // Wait longer to send image
        CFE_EVS_SendEvent(TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
                          "TIM: Send Camera 0 File Yielding, WC = [%d]",
                          image_wait_count);
      }
    }
    break;

  // Received send image command from camera 1
  case TIM_APP_SEND_IMAGE1_CC:
    // If last command was a temperature file and serial isn't busy, send an
    // image
    if (serial_last_sent == TIM_APP_SEND_TEMPS_CC) {
      if (serial_busy == 0) {
        // Set serial to busy while transmitting, set last sent command to this
        serial_busy = 1;
        serial_last_sent = TIM_APP_SEND_IMAGE1_CC;

        // Send image
        TIM_SendImageFile();
        // Mark serial as ready once complete
        serial_busy = 0;

        // Send event to CFE indicating this command was received
        CFE_EVS_SendEvent(TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
                          "TIM: Send Camera 1 File Command Received");

        // Increment related command counts on telemetry packet
        TIM_HkTelemetryPkt.tim_command_count++;
        TIM_HkTelemetryPkt.tim_command_image_count++;

        // Reset Wait Count when we know its been sent
        image_wait_count = 0;

      } else {
        // Send CFE event indicating we received send command, but serial was
        // busy
        CFE_EVS_SendEvent(
            TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
            "TIM: Send Camera 1 File Command Received but Serial Busy");
      }

    } else {
      // Increment wait count since last image
      image_wait_count++;

      // If we've waited the maximum times since last image, try to send one
      if (image_wait_count >= TIM_IMAGE_MAX_COUNT) {
        if (serial_busy == 0) {
          /*
          * See above for documentation on this code block. I've now documented
          * basically this same block three times, and I'm not doing a fourth.
          * TODO: move these blocks into a generalized function.
          */
          serial_busy = 1;
          serial_last_sent = TIM_APP_SEND_IMAGE1_CC;
          TIM_SendImageFile();
          serial_busy = 0;

          CFE_EVS_SendEvent(TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
                            "TIM: Send Camera 1 File Command Received");

          TIM_HkTelemetryPkt.tim_command_count++;
          TIM_HkTelemetryPkt.tim_command_image_count++;

          // Reset Wait Count once we know it's been sent
          image_wait_count = 0;

        } else {
          // Send event indicating we received the command but serial was busy
          CFE_EVS_SendEvent(TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
                            "TIM: Send Camera 1 File Command Received but "
                            "Serial Busy, WC = [%d]",
                            image_wait_count);
        }

      } else {
        // Send event indicating we're waiting
        CFE_EVS_SendEvent(TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
                          "TIM: Send Camera 1 File yeilding, Wait Count = [%d]",
                          image_wait_count);
      }
    }
    break;

  // Send temperature file command received from ADS1115
  case TIM_APP_SEND_TEMPS_CC:
    // If the last sent wasn't temperatures as well and serial isn't busy, send
    // temperatures
    if (serial_last_sent != TIM_APP_SEND_TEMPS_CC) {
      if (serial_busy == 0) {
        // Mark serial as busy while transmitting temperatures and mark last
        // sent as this command
        serial_busy = 1;
        serial_last_sent = TIM_APP_SEND_TEMPS_CC;

        // Send the temperature file
        TIM_SendTempsFile();

        // Mark serial as ready once complete
        serial_busy = 0;

        // Send an event to the CFE indicating we received the send temperature
        // file command
        CFE_EVS_SendEvent(TIM_COMMAND_TEMPS_EID, CFE_EVS_INFORMATION,
                          "TIM: Send Temps File Command Received");
        // Increment related command counts on telemetry packet
        TIM_HkTelemetryPkt.tim_command_count++;
        TIM_HkTelemetryPkt.tim_command_temps_count++;

        // Reset wait count once we know it's been sent
        temp_wait_count = 0;
      } else {
        // Send an event to the CFE indicating we received the command but
        // serial was busy
        CFE_EVS_SendEvent(
            TIM_COMMAND_TEMPS_EID, CFE_EVS_INFORMATION,
            "TIM: Send Temps File Command Received but Serial Busy");
      }
    } else {
      // Increment wait count since last temperature file transmission
      temp_wait_count++;

      // If we've waited the max time and serial isn't busy, send file
      // regardless of the last transmission not being an image
      if (temp_wait_count >= TIM_TEMP_MAX_COUNT) {
        if (serial_busy == 0) {
          // Mark serial as busy and last sent as this command
          serial_busy = 1;
          serial_last_sent = TIM_APP_SEND_TEMPS_CC;

          // Send temperatures
          TIM_SendTempsFile();

          // Mark serial as ready once complete
          serial_busy = 0;

          // Send CFE event indicating temperature wait limit was reached,
          // sending file
          CFE_EVS_SendEvent(
              TIM_COMMAND_TEMPS_EID, CFE_EVS_INFORMATION,
              "TIM: Reached Temp Wait Limit. Force Sending Temps File");

          // Increment related command counts on telemetry packet
          TIM_HkTelemetryPkt.tim_command_count++;
          TIM_HkTelemetryPkt.tim_command_temps_count++;

          // Reset Wait Count when we know its been sent
          temp_wait_count = 0;
        } else {
          // Send CFE event indicating the wait limit was reached but serial was
          // busy
          CFE_EVS_SendEvent(TIM_COMMAND_TEMPS_EID, CFE_EVS_INFORMATION,
                            "TIM: Reached Temp Wait Limit But Serial Busy");
        }
      } else {
        // Send CFE event indicating we're waiting, and the current wait count
        CFE_EVS_SendEvent(TIM_COMMAND_TEMPS_EID, CFE_EVS_INFORMATION,
                          "TIM: Send Temps File Wait Count Reached [%d]",
                          temp_wait_count);
      }
    }

    break;

  // Case for unknown commands
  default:
    // Tell CFE we got an unknown command
    CFE_EVS_SendEvent(TIM_COMMAND_ERR_EID, CFE_EVS_INFORMATION,
                      "Command code not found (CC = %i)", CommandCode);
    // Increment command error count in telemetry packet
    TIM_HkTelemetryPkt.tim_command_error_count++;
    break;
  }
}

/** Triggered in response to a task telemetry request from the housekeeping
 *  task. Gathers the app's telemetry, packetizes it and sends it to the
 *  housekeeping task via the software bus.
 */
void TIM_ReportHousekeeping(void) {
  /*
      memcpy((char *) &TIM_HkTelemetryPkt.SerialQueueInfo,
              (char *) &TIM_SerialQueueInfo,
              sizeof(TIM_SerialQueue_t));
  */

  // Cast TIM_HkTelemetryPkt to a CFE software bus message and timestamp it
  CFE_SB_TimeStampMsg((CFE_SB_Msg_t *)&TIM_HkTelemetryPkt);
  // Cast TIM_HkTelemetryPkt to a CFE software bus message and send it
  CFE_SB_SendMsg((CFE_SB_Msg_t *)&TIM_HkTelemetryPkt);
}

/** This function resets all the global counter variables that are part of the
 *  task telemetry. Also clears file loc data.
 */
void TIM_ResetCounters(void) {
  // Status of commands processed by the TIM App
  TIM_HkTelemetryPkt.tim_command_count = 0;
  TIM_HkTelemetryPkt.tim_command_error_count = 0;
  TIM_HkTelemetryPkt.tim_command_image_count = 0;
  TIM_HkTelemetryPkt.tim_command_temps_count = 0;

  // Clear file data for last image and temperature files
  memset(TIM_HkTelemetryPkt.tim_last_image_sent, '\0',
         sizeof(TIM_HkTelemetryPkt.tim_last_image_sent));
  memset(TIM_HkTelemetryPkt.tim_last_temps_sent, '\0',
         sizeof(TIM_HkTelemetryPkt.tim_last_temps_sent));

  // Tell CFE that we got a reset command
  CFE_EVS_SendEvent(TIM_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
                    "TIM: RESET command");
}

/** Sends a picture using a JPG file, based on info from the global
 *  (CFE_SB_MsgPtr_t *) TIMMsgPtr
 *  TODO: make TIMMsgPtr not a global, and pass it into this function?
 */
void TIM_SendImageFile(void) {
  /** Holds the return value of write operations
   *  TODO: This can easily be optimized out.
   */
  int os_ret_val = 0;
  // int32 os_fd = 0;
  // uint32 bytes_per_read = 1; /* const */
  // uint16 total_bytes_read = 0;
  // uint8 data_buf[2];
  // data_buf[0] = 0;
  // data_buf[1] = 0;

  // mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  /* Creating a pointer to handle the TIMMsgPtr as TIM_IMAGE_CMD_PKT  */

  TIM_IMAGE_CMD_PKT_t *ImageCmdPtr;
  ImageCmdPtr = (TIM_IMAGE_CMD_PKT_t *)TIMMsgPtr;

  // OS_printf("Received TIM_SendImageFile() with %s\n",
  // ImageCmdPtr->ImageName);

  char file_path[OS_MAX_PATH_LEN];

  // OS_printf("sizeof(file_path[OS_MAX_PATH_LEN]) = %d\n", sizeof(file_path));

  memset(file_path, '\0', sizeof(file_path));

  if ((os_ret_val = snprintf(file_path, sizeof(file_path), "/ram/images/%s",
                             ImageCmdPtr->ImageName)) < 0) {
    OS_printf("TIM snprintf failure: ret = %d\n", os_ret_val);
  }

  /*
  if ((os_fd = OS_open((const char * ) file_path, (int32) OS_READ_ONLY, (uint32)
  mode)) < OS_FS_SUCCESS)
  {
      OS_printf("TIM: OS_open Returned [%d] (expected non-negative value).\n",
  os_fd);
      return;
  }
  */

  /*
  ** Read 1 byte at a time
  ** Uncomment the following to o
  */
  /*
  while( OS_read((int32) os_fd, (void *) data_buf, (uint32) bytes_per_read) &&
  (total_bytes_read < 20000) )
  {
      //OS_printf("From Tim Image: File '%s' Byte %.2d = %#.2X %#.2X\n",
  ImageCmdPtr->ImageName, total_bytes_read, data_buf[0], data_buf[1]);
      total_bytes_read++;
      //serial_write_byte(&TIM_SerialUSB, (unsigned char) data_buf[0]);
      data_buf[0] = 0;
      data_buf[1] = 0;
  }
  OS_printf("Image File Length (bytes) = [%u].\n", total_bytes_read);
  */

  // OS_printf("Image File Length (bytes) = [%u].\n", total_bytes_read);

  /*
  uint8 file_len[2];
  file_len[0] = *((uint8 *) &total_bytes_read);
  file_len[1] = *(((uint8 *) &total_bytes_read) + 1);
  OS_printf("Image Length MSB = [%#.2X]\n", file_len[1]);
  OS_printf("Image Length LSB = [%#.2X]\n", file_len[0]);
  //OS_printf("Image Length MSB = [%#.2X]\n", file_len[1]);
  //OS_printf("Image Length LSB = [%#.2X]\n", file_len[0]);


  OS_close(os_fd);

  /*
  ** Data prepped for serial:
  ** total_bytes_read : file size
  ** TempsCmdPtr->TempsName : file name
  ** sizeof(TempsCmdPtr->TempsName) : 22
  */

  /*
  uint8 start byte and pkt id  {0xF} {0:?, 1:image, 2:temps, 3:log, 4:unknown}
  uint16 pkt size in bytes
  uint8 filename length
  char filename[22]
  blank (0x00)
  ...
  DATA
  ...
  uint16 data_stop_flag 0xFF [filename length]
  char filename[22]
  uint8 stop byte
  */

  // OS_close(os_fd);

  // TODO: Figure out what these magic numbers do. Ask Keegan?
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xFF);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xF2);

  // serial_write_byte(&TIM_SerialUSB, (unsigned char) file_len[0]);
  // serial_write_byte(&TIM_SerialUSB, (unsigned char) file_len[1]); /* check
  // endianess */

  // Write image name to serial?
  // TODO: Ask Keegan for confirmation.
  for (int index = 0; index < sizeof(ImageCmdPtr->ImageName); index++) {
    serial_write_byte(
        &TIM_SerialUSB,
        (unsigned char)(*(((char *)ImageCmdPtr->ImageName) + index)));
  }

  serial_write_byte(&TIM_SerialUSB, (unsigned char)0x00);

  // Write image file to serial
  tim_serial_write_file(&TIM_SerialUSB, (char *)file_path);

  // TODO: Figure out what these magic numbers do. Ask Keegan?
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xFF);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xF3);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0x0D);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0x0A);

  // Tell CFE the image was sent successfully
  CFE_EVS_SendEvent(TIM_COMMAND_IMAGE_EID, CFE_EVS_INFORMATION,
                    "Image File \'%s\' Sent Successfully", file_path);

  // OS_printf("Reached end of TIM_SendImageFile().\n");
}

/** Sends temperature file
 *  TODO: consider moving the common logic between this and sending camera
 *  images into its own function.
 */
void TIM_SendTempsFile(void) {
  /** Holds return value of write operations.
   *  TODO: This can easily be optimized out.
   */
  int os_ret_val = 0;
  /*
  int32 os_fd = 0;
  uint32 bytes_per_read = 1;
  uint16 total_bytes_read = 0;
  uint8 data_buf[2];
  data_buf[0] = 0;
  data_buf[1] = 0;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  */

  /* Creating a pointer to handle the TIMMsgPtr as TIM_IMAGE_CMD_PKT  */
  TIM_TEMPS_CMD_PKT_t *TempsCmdPtr;
  TempsCmdPtr = (TIM_TEMPS_CMD_PKT_t *)TIMMsgPtr;

  // OS_printf("Received TIM_SendTempsFile() with %s\n",
  // TempsCmdPtr->TempsName);

  /** File path to save the image to */
  char file_path[OS_MAX_PATH_LEN];

  // OS_printf("sizeof(file_path[OS_MAX_PATH_LEN]) = %d\n", sizeof(file_path));

  memset(file_path, '\0', sizeof(file_path));

  if ((os_ret_val = snprintf(file_path, sizeof(file_path), "/ram/temps/%s",
                             TempsCmdPtr->TempsName)) < 0) {
    OS_printf("TIM snprintf failure: ret = %d\n", os_ret_val);
  }

  /*
  if ((os_fd = OS_open((const char * ) file_path, (int32) OS_READ_ONLY, (uint32)
  mode)) < OS_FS_SUCCESS)
  {
      OS_printf("TIM: OS_open Returned [%d] (expected non-negative value).\n",
  os_fd);
      return;
  }
  */

  /*
  ** Read 1 byte at a time
  */
  /*

  while( OS_read((int32) os_fd, (void *) data_buf, (uint32) bytes_per_read))
  {
      //OS_printf("From Tim Temps: File '%s' Byte %.2d = %#.2X %#.2X\n",
  TempsCmdPtr->TempsName, total_bytes_read, data_buf[0], data_buf[1]);
      total_bytes_read++;
      //serial_write_byte(&TIM_SerialUSB, (unsigned char) data_buf[0]);
      data_buf[0] = 0;
      data_buf[1] = 0;
  }

  if(total_bytes_read != 16)
  {
      OS_printf("Temperature File shorter [%d] than expected [16].\n",
  total_bytes_read);
  }

  uint8 file_len[2];
  file_len[0] = *((uint8 *) &total_bytes_read);
  file_len[1] = *(((uint8 *) &total_bytes_read) + 1);
  OS_printf("Temp Length MSB = [%#.2X]\n", file_len[1]);
  OS_printf("Temp Length LSB = [%#.2X]\n", file_len[0]);
  //OS_printf("Image Length MSB = [%#.2X]\n", file_len[1]);
  //OS_printf("Image Length LSB = [%#.2X]\n", file_len[0]);

  OS_close(os_fd);

  */

  // TODO: Maaaagic numbers, as with the last function. Ask Keegan.
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xFF);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xF4);

  // serial_write_byte(&TIM_SerialUSB, (unsigned char) file_len[1]);
  // serial_write_byte(&TIM_SerialUSB, (unsigned char) file_len[0]); /* check
  // endianess */

  // Write filename to serial?
  // TODO: Ask Keegan for confirmation.
  for (int index = 0; index < sizeof(TempsCmdPtr->TempsName); index++) {
    serial_write_byte(
        &TIM_SerialUSB,
        (unsigned char)(*(((char *)TempsCmdPtr->TempsName) + index)));
  }

  serial_write_byte(&TIM_SerialUSB, (unsigned char)0x00);

  // Write file to serial
  tim_serial_write_file(&TIM_SerialUSB, (char *)file_path);

  // TODO: more magic numbers that need documenting.
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xFF);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xF5);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0x0D);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0x0A);

  // Tell CFE we've sent the temperature file successfully.
  CFE_EVS_SendEvent(TIM_COMMAND_TEMPS_EID, CFE_EVS_INFORMATION,
                    "Temps File \'%s\' Sent Successfully", file_path);
}

/** Sends a log file, as the name indicates. */
void TIM_SendLogFile(void) {
  /** Path to log to */
  char log_path[] = "/ram/logs/CFS_Boot.out";
  /** Filename of the log file */
  char logs_filename[] = "CFS_Boot.out";

  // OS_printf("TIM_APP: Sending Log file.\n");

  /*
  int32 os_fd = 0;
  uint32 bytes_per_read = 1;
  uint16 total_bytes_read = 0;
  uint8 data_buf[2];
  data_buf[0] = 0;
  data_buf[1] = 0;
  mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  if ((os_fd = OS_open((const char * ) log_path, (int32) OS_READ_ONLY, (uint32)
  mode)) < OS_FS_SUCCESS)
  {
      OS_printf("TIM::SendLogFile(): OS_open Returned [%d] (expected
  non-negative value).\n", os_fd);
      return;
  }
  */

  /*
  ** Read 1 byte at a time
  */
  /*
  while( OS_read((int32) os_fd, (void *) data_buf, (uint32) bytes_per_read))
  {
      //OS_printf("From Tim Temps: File '%s' Byte %.2d = %#.2X %#.2X\n",
  TempsCmdPtr->TempsName, total_bytes_read, data_buf[0], data_buf[1]);
      total_bytes_read++;
      //serial_write_byte(&TIM_SerialUSB, (unsigned char) data_buf[0]);
      data_buf[0] = 0;
      data_buf[1] = 0;
  }

      OS_printf("TIM: Log File size: [%d] bytes.\n", total_bytes_read);


  uint8 file_len[2];
  file_len[0] = *((uint8 *) &total_bytes_read);
  file_len[1] = *(((uint8 *) &total_bytes_read) + 1);
  OS_printf("Image Length MSB = [%#.2X]\n", file_len[1]);
  OS_printf("Image Length LSB = [%#.2X]\n", file_len[0]);

  OS_close(os_fd);
  */

  // TODO: magic numbers.
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xFF);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xF6);

  // serial_write_byte(&TIM_SerialUSB, (unsigned char) file_len[1]);
  // serial_write_byte(&TIM_SerialUSB, (unsigned char) file_len[0]); /* check
  // endianess */

  // Write logs filename to serial
  // TODO: get Keegan to confirm this
  for (int index = 0; index < sizeof(log_path); index++) {
    serial_write_byte(&TIM_SerialUSB, (unsigned char)logs_filename[index]);
  }

  serial_write_byte(&TIM_SerialUSB, (unsigned char)0x00);

  // Write log file to serial
  tim_serial_write_file(&TIM_SerialUSB, (char *)log_path);

  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xFF);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0xF7);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0x0D);
  serial_write_byte(&TIM_SerialUSB, (unsigned char)0x0A);

  // Tell CFE we sent the log file
  CFE_EVS_SendEvent(TIM_COMMAND_LOGS_EID, CFE_EVS_INFORMATION,
                    "Log File \'%s\' Sent Successfully", log_path);
}

/** Verifies the length of the command packet
 *  @param[in] msg The message to check the length of
 *  @param[in] ExpectedLength The expected length of the message
 */
boolean TIM_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength) {
  // Get length of the command packet
  uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

  // Make sure the command packet's length matches what we expect
  if (ExpectedLength != ActualLength) {
    CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(msg);
    uint16 CommandCode = CFE_SB_GetCmdCode(msg);

    // Send an event indicating the actual and expected values for the command
    CFE_EVS_SendEvent(
        TIM_LEN_ERR_EID, CFE_EVS_ERROR,
        "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
        MessageID, CommandCode, ActualLength, ExpectedLength);
    // Increment error count on telemetry packet
    TIM_HkTelemetryPkt.tim_command_error_count++;
    return FALSE;
  } else {
    // Length is valid
    return TRUE;
  }
}

/** Initializes parallel communications */
void TIM_Parallel_Init(void) {
  // Set up wiring library
  wiringPiSetup();

  // Set the parallel power pin to OUTPUT and set it to high
  pinMode(TIM_PAR_GPIO_PIN_PWR, OUTPUT);
  digitalWrite(TIM_PAR_GPIO_PIN_PWR, HIGH);

  // Tell CFE we turned on the parallel power pin
  CFE_EVS_SendEvent(TIM_PARALLEL_PWR_EID, CFE_EVS_INFORMATION,
                    "Parallel Power Flag Set");

  // Set the parallel TE-1 pin to OUTPUT and set it to low
  pinMode(TIM_PAR_GPIO_PIN_TE, OUTPUT);
  digitalWrite(TIM_PAR_GPIO_PIN_TE, LOW);

  // Tell the CFE we initialized the TE-1 flag at low
  CFE_EVS_SendEvent(TIM_PARALLEL_TE_EID, CFE_EVS_INFORMATION,
                    "Parallel TE-1 Flag Initialized Low");

  // Initialize TE-1 digital input with built-in pulldown resistor
  // Set the TE-1 pin to input
  pinMode(TIM_MAIN_GPIO_PIN_TE, INPUT);
  // Set the pull on the pin to down
  pullUpDnControl(TIM_MAIN_GPIO_PIN_TE, PUD_DOWN);

  // Set the parallel pin to the value of the TE-1 input pin
  if (digitalRead(TIM_MAIN_GPIO_PIN_TE) == HIGH) {
    digitalWrite(TIM_PAR_GPIO_PIN_TE, HIGH);
    // Tell CFE that TE-1 is high
    CFE_EVS_SendEvent(TIM_PARALLEL_TE_EID, CFE_EVS_INFORMATION,
                      "Parallel TE-1 Flag Set");
  } else {
    digitalWrite(TIM_PAR_GPIO_PIN_TE, LOW);
    // Tell CFE that TE-1 is low
    CFE_EVS_SendEvent(TIM_PARALLEL_TE_EID, CFE_EVS_INFORMATION,
                      "Parallel TE-1 Flag Cleared");
  }
}

/** Updates the timer event from the TE-1 input */
void TIM_UpdateTimerEvent(void) {
  // If TE is high and the last event wasn't, update the parallel pin
  if (digitalRead(TIM_MAIN_GPIO_PIN_TE) == HIGH) {
    if (digitalRead(TIM_MAIN_GPIO_PIN_TE) != timer_event_last) {
      // Tell CFE we have a new value
      CFE_EVS_SendEvent(TIM_PARALLEL_TE_EID, CFE_EVS_INFORMATION,
                        "TE-1 UPDATE: New Value");
      // Write new value to parallel
      digitalWrite(TIM_PAR_GPIO_PIN_TE, HIGH);
      // Tell CFE we successfully wrote the new value
      CFE_EVS_SendEvent(TIM_PARALLEL_TE_EID, CFE_EVS_INFORMATION,
                        "TE-1 UPDATE: TE-1 Set");
    }
    // Set last timer event to reflect this
    timer_event_last = HIGH;
    // If TE is low and the last event wasn't, update the parallel pin
  } else {
    if (digitalRead(TIM_MAIN_GPIO_PIN_TE) != timer_event_last) {
      // Tell CFE we have a new value
      CFE_EVS_SendEvent(TIM_PARALLEL_TE_EID, CFE_EVS_INFORMATION,
                        "TE-1 UPDATE: New Value");
      // Write new value to parallel
      digitalWrite(TIM_PAR_GPIO_PIN_TE, LOW);
      // Tell CFE we successfully wrote the new value
      CFE_EVS_SendEvent(TIM_PARALLEL_TE_EID, CFE_EVS_INFORMATION,
                        "TE-1 UPDATE: TE-1 Cleared");
    }
    // Set last timer event to reflect this
    timer_event_last = LOW;
  }
}

/** Power off the system, called by a timer created at app initialization.
 *  @param[in] poweroff_timer_id The ID of the power-off timer calling this.
 */
void timer_callback_poweroff_system(uint32 poweroff_timer_id) {
  // announce timer to everone logged into the system -- purely for debugging.
  system("wall -n TRAPSat_CFS: timer_callback_poweroff_system called!");

  // Send Log files before poweroff from Wallops
  while (serial_busy) {
    // wait until serial not busy
  }
  serial_busy = 1; // Occupy Serial for sending of logs file
  TIM_SendLogFile();
  serial_busy = 0;
  // poweroff the system
  // system("shutdown -P now");

  OS_printf("TIM_APP: leaving cFS\n");
  // CFE_PSP_SigintHandler(); // End cFS -- Obviously, this will only happen
  // when Pi is on.
}