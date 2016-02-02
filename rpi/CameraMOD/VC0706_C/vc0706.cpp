/*
 * Retrieved from https://github.com/vyykn/VC0706
 *
 *
 */

//#include <node.h> // removed Node code -- Zach
//#include <node_object_wrap.h>

// C standard library
#include <cstdlib>
#include <ctime>
#include <errno.h>

#include <stdint.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

// Brian's DEPS
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
//using namespace v8;
//using namespace node;


//extern "C"  {
//#include "deps/wiringSerial.h"
//#include "deps/wiringPi.h"
//}

#include <wiringSerial.h>
#include <wiringPi.h>


// Brian's Defines
#define BAUD 38400
//#define BAUD 115200

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
#define TO_U 500000 //test
//#define TO_U 1000000



class Camera {

public:
    Camera();
    ~Camera();
    void reset();
    void setMotionDetect(int flag);
    bool checkReply(int cmd, int size);
    void clearBuffer();
    bool motionDetected();
    char * getVersion();
    char * takePicture();
    int motion;
    int ready;
    int fd;

private:
    void resumeVideo();

    uint8_t offscreen[8]; // font width;

    int frameptr;
    int bufferLen;
    int serialNum;
    char camerabuff[CAMERABUFFSIZ+1];
    char serialHeader[5];
    char imageName[16];
    char * empty;
};

Camera::Camera() {
    frameptr = 0;
    bufferLen = 0;
    serialNum = 0;
    motion = 1;
    ready = 1;


    if ((fd = serialOpen("/dev/ttyAMA0", BAUD)) < 0)
        cout <<  "SPI Setup Failed: " <<  strerror(errno) << endl;

    if (wiringPiSetup() == -1)
        exit(1);


    ready = 1;

}

Camera::~Camera() {}

void Camera::reset() {
    // Camera Reset method
    serialPutchar(fd, (char)0x56);
    serialPutchar(fd, (char)serialNum);
    serialPutchar(fd, (char)RESET);
    serialPutchar(fd, (char)0x00);

    if (checkReply(RESET, 5) != true)
        cout <<  "Check Reply Status: " <<  strerror(errno) << endl;

    clearBuffer();

}

bool Camera::checkReply(int cmd, int size) {
    int reply[size];
    int t_count = 0;
    int length = 0;
    int avail = 0;
    int timeout = 5 * TO_SCALE;
    
    while ((timeout != t_count) && (length != CAMERABUFFSIZ) && length < size)
    {
        avail = serialDataAvail(fd);
        if (avail <= 0)
        {
            usleep(TO_U);
            t_count++;
            continue;
        }
        t_count = 0;
        // there's a byte!
        int newChar = serialGetchar(fd);
        reply[length++] = (char)newChar;
    }
    
    //Check the reply
    if (reply[0] != 0x76 && reply[1] != 0x00 && reply[2] != cmd)
        return false;
    else
        return true;
}

void Camera::clearBuffer() {
    int t_count = 0;
    int length = 0;
    int timeout = 2 * TO_SCALE;

    while ((timeout != t_count) && (length != CAMERABUFFSIZ))
    {
        int avail = serialDataAvail(fd);
        if (avail <= 0)
        {
            t_count++;
            continue;
        }
        t_count = 0;
        // there's a byte!
        serialGetchar(fd);
        length++;
    }
}

void Camera::resumeVideo()
{
    serialPutchar(fd, (char)0x56);
    serialPutchar(fd, (char)serialNum);
    serialPutchar(fd, (char)FBUF_CTRL);
    serialPutchar(fd, (char)0x01);
    serialPutchar(fd, (char)RESUMEFRAME);
    
    if (checkReply(FBUF_CTRL, 5) == false)
        printf("Camera did not resume\n");
}

char * Camera::getVersion()
{
    serialPutchar(fd, (char)0x56);
    serialPutchar(fd, (char)serialNum);
    serialPutchar(fd, (char)GEN_VERSION);
    serialPutchar(fd, (char)0x00);
    
    if (checkReply(GEN_VERSION, 5) == false)
    {
        printf("CAMERA NOT FOUND!!!\n");
    }
    
    int counter = 0;
    bufferLen = 0;
    int avail = 0;
    int timeout = 1 * TO_SCALE;
    
    while ((timeout != counter) && (bufferLen != CAMERABUFFSIZ))
    {
        avail = serialDataAvail(fd);
        if (avail <= 0)
        {
            usleep(TO_U);
            counter++;
            continue;
        }
        counter = 0;
        // there's a byte!
        int newChar = serialGetchar(fd);
        camerabuff[bufferLen++] = (char)newChar;
    }
    
    camerabuff[bufferLen] = 0;
    
    return camerabuff;
}

void Camera::setMotionDetect(int flag)
{
    serialPutchar(fd, (char)0x56);
    serialPutchar(fd, (char)0x00);
    serialPutchar(fd, (char)0x42);
    serialPutchar(fd, (char)0x04);
    serialPutchar(fd, (char)0x01);
    serialPutchar(fd, (char)0x01);
    serialPutchar(fd, (char)0x00);
    serialPutchar(fd, (char)0x00);
    
    serialPutchar(fd, (char)0x56);
    serialPutchar(fd, (char)serialNum);
    serialPutchar(fd, (char)COMM_MOTION_CTRL);
    serialPutchar(fd, (char)0x01);
    serialPutchar(fd, (char)flag);

    clearBuffer();

}


char * Camera::takePicture()
{
    frameptr = 0;

    // Force Stop motion detect
    setMotionDetect(0);

    //Clear Buffer
    clearBuffer();

    serialPutchar(fd, (char)0x56);
    serialPutchar(fd, (char)serialNum);
    serialPutchar(fd, (char)FBUF_CTRL);
    serialPutchar(fd, (char)0x01);
    serialPutchar(fd, (char)STOPCURRENTFRAME);
    
    if (checkReply(FBUF_CTRL, 5) == false)
    {
        printf("Frame checkReply Failed\n");
        return empty;
    }

    
    serialPutchar(fd, (char)0x56);
    serialPutchar(fd, (char)serialNum);
    serialPutchar(fd, (char)GET_FBUF_LEN);
    serialPutchar(fd, (char)0x01);
    serialPutchar(fd, (char)0x00);
    
    if (checkReply(GET_FBUF_LEN, 5) == false)
    {
        printf("FBUF_LEN REPLY NOT VALID!!!\n");
        return empty;
    }
    
    while(serialDataAvail(fd) <= 0){}

    printf("Serial Data Avail %u \n", serialDataAvail(fd));
    
    int len;
    len = serialGetchar(fd);
    len <<= 8;
    len |= serialGetchar(fd);
    len <<= 8;
    len |= serialGetchar(fd);
    len <<= 8;
    len |= serialGetchar(fd);

    printf("Length %u \n", len);

    if(len > 20000){
        printf("To Large... \n");
        resumeVideo();
        clearBuffer();
        return Camera::takePicture();
    }
    
    char image[len];
    
    int imgIndex = 0;
    
    while (len > 0)
    {
        int readBytes = len;
        
        serialPutchar(fd, (char)0x56);
        serialPutchar(fd, (char)serialNum);
        serialPutchar(fd, (char)READ_FBUF);
        serialPutchar(fd, (char)0x0C);
        serialPutchar(fd, (char)0x0);
        serialPutchar(fd, (char)0x0A);
        serialPutchar(fd, (char)(frameptr >> 24 & 0xff));
        serialPutchar(fd, (char)(frameptr >> 16 & 0xff));
        serialPutchar(fd, (char)(frameptr >> 8 & 0xff));
        serialPutchar(fd, (char)(frameptr & 0xFF));
        serialPutchar(fd, (char)(readBytes >> 24 & 0xff));
        serialPutchar(fd, (char)(readBytes >> 16 & 0xff));
        serialPutchar(fd, (char)(readBytes >> 8 & 0xff));
        serialPutchar(fd, (char)(readBytes & 0xFF));
        serialPutchar(fd, (char)(CAMERADELAY >> 8));
        serialPutchar(fd, (char)(CAMERADELAY & 0xFF));
            
        if (checkReply(READ_FBUF, 5) == false)
        {
            return empty;
        }
        
        int counter = 0;
        bufferLen = 0;
        int avail = 0;
        int timeout = 20 * TO_SCALE;
        
        while ((timeout != counter) && bufferLen < readBytes)
        {
            avail = serialDataAvail(fd);

            if (avail <= 0)
            {
                usleep(TO_U);
                counter++;
                continue;
            }
            counter = 0;
            int newChar = serialGetchar(fd);
            image[imgIndex++] = (char)newChar;
            
            bufferLen++;
        }
        
        frameptr += readBytes;
        len -= readBytes;
        
        if (checkReply(READ_FBUF, 5) == false)
        {
            printf("ERROR READING END OF CHUNK| start: %u | length: %u\n", frameptr, len);
        }
    }

    
    char name[23];
    int t = (int)time(NULL);
    sprintf(name, "images/%u.jpg", t);
    
    FILE *jpg = fopen(name, "w");
    if (jpg != NULL)
    {
        fwrite(image, sizeof(char), sizeof(image), jpg);
        fclose(jpg);
    }
    else
    {
        printf("IMAGE COULD NOT BE OPENED/MADE!\n");
    }
    
    sprintf(imageName, "%u.jpg", t);
    
    resumeVideo();

    //Clear Buffer
    clearBuffer();

    // Force Stop motion detect
    setMotionDetect(0);

    
    return imageName;
    
}

// End Brian's Methods

// There once was a bunch of Node JS code here .... it is gone now -- Zach
