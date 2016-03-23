/*
 * Retrieved from https://github.com/vyykn/VC0706
 *
 * Edited By Zach Richard for use on TRAPSat aboard the RockSat-X 2016 Mission
 */
#ifndef _vc0706_core_h_
#define _vc0706_core_h_

#include "vc0706.h"
/*
#include <time.h>
#include <errno.h>
#include <stdbool.h>

#include <stdint.h>
#include <stdlib.h>

// Brian's DEPS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <wiringSerial.h>
#include <wiringPi.h>
*/

// Brian's Defines
#define BAUD 38400

#define RESET 0x26
#define GEN_VERSION 0x11
#define READ_FBUF 0x32
#define GET_FBUF_LEN 0x34
#define FBUF_CTRL 0x36
#define DOWNSIZE_CTRL 0x54
#define DOWNSIZE_STATUS 0x55
#define READ_DATA 0x30
#define WRITE_DATA 0x31
#define COMM_MOTION_CTRL 0x37
#define COMM_MOTION_STATUS 0x38
#define COMM_MOTION_DETECTED 0x39
#define MOTION_CTRL 0x42
#define MOTION_STATUS 0x43
#define TVOUT_CTRL 0x44
#define OSD_ADD_CHAR 0x45

#define STOPCURRENTFRAME 0x0
#define STOPNEXTFRAME 0x1
#define RESUMEFRAME 0x3
#define STEPFRAME 0x2

#define SIZE640 0x00
#define SIZE320 0x11
#define SIZE160 0x22

#define MOTIONCONTROL 0x0
#define UARTMOTION 0x01
#define ACTIVATEMOTION 0x01

#define SET_ZOOM 0x52
#define GET_ZOOM 0x53

#define CAMERABUFFSIZ 100
#define CAMERADELAY 10
#define TO_SCALE 1
#define TO_U 200000
//#define TO_U 1000000

typedef struct Camera_t {
    int motion;
    int ready;
    int fd;

    int frameptr;
    int bufferLen;
    int serialNum;
    char camerabuff[CAMERABUFFSIZ+1];
    char serialHeader[5];
    char imageName[OS_MAX_PATH_LEN]; // cFS defined
    char * empty;
} Camera_t;


int init(Camera_t *cam);
bool checkReply(Camera_t *cam, int cmd, int size);
void clearBuffer(Camera_t *cam);
void reset(Camera_t *cam);
void resumeVideo(Camera_t *cam);
int  getVersion(Camera_t *cam);
void setMotionDetect(Camera_t *cam, int flag);
char * takePicture(Camera_t *cam, char * file_path);

#endif /* _vc0706_core_h__ */
