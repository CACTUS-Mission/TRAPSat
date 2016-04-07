/*
 * Retrieved from https://github.com/vyykn/VC0706
 *
 * Edited By Zach Richard for use on TRAPSat aboard the RockSat-X 2016 Mission
 */

#include "vc0706_core.h"

/*
** External led structure
*/
extern struct led_t led;

/*
** External mux structure -- for error data
*/
extern struct mux_t mux;



int init(Camera_t *cam) {
    cam->frameptr = 0;
    cam->bufferLen = 0;
    cam->serialNum = 0;
    cam->motion = 1;
    cam->ready = 1;
    cam->empty = "";
    if ((cam->fd = serialOpen("/dev/ttyAMA0", BAUD)) < 0)
    {
        //fprintf(stderr, "SPI Setup Failed: %s\n", strerror(errno));
    	CFE_EVS_SendEvent(VC0706_CHILD_INIT_ERR_EID, CFE_EVS_ERROR, "init Error: Failed to open specified port at %s. STDERR: %s", "/dev/ttyAMA0", strerror(errno));
    	return -1;
    }

    if (wiringPiSetup() == -1)
    {
        //OS_printf("wiringPiSetup(0 failed.\n");
        CFE_EVS_SendEvent(VC0706_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
            "init Error: wiringPiSetup() failed.");
	    return -1;
    }

    cam->ready = 1;
	return 0;
}

bool checkReply(Camera_t *cam, int cmd, int size) {
    int reply[size];
    int t_count = 0;
    int length = 0;
    int avail = 0;
    int timeout = 3 * TO_SCALE; // test 3 was 5

    while ((timeout != t_count) && (length != CAMERABUFFSIZ) && length < size)
    {
        avail = serialDataAvail(cam->fd);
        if (avail <= 0)
        {
            usleep(TO_U);
            t_count++;
            continue;
        }
        t_count = 0;
        // there's a byte!
        int newChar = serialGetchar(cam->fd);
        reply[length++] = (char)newChar;
    }

    //Check the reply
    if (reply[0] != 0x76 || reply[1] != 0x00 || reply[2] != cmd)
    {
        //CFE_EVS_SendEvent(VC0706_REPLY_ERR_EID, CFE_EVS_ERROR,"Camera %d unresponsive. reply[0]: %x, expected: %x reply[1]: %x, expected: %x", mux.mux_state, reply[0], 0x76, reply[1], 0x00);
        //CFE_EVS_SendEvent(VC0706_REPLY_ERR_EID, CFE_EVS_ERROR,"\treply[2]: %x, expected: %x STRERROR: %s", reply[2], cmd, strerror(errno));
        //CFE_EVS_SendEvent(VC0706_REPLY_ERR_EID, CFE_EVS_ERROR,"Camera %d unresponsive!", mux.mux_state);
        CFE_EVS_SendEvent(VC0706_REPLY_ERR_EID, CFE_EVS_ERROR,"Camera %d unresponsive! R[0] = [%x] R[1] = [%x] R[2] = [%x]", mux.mux_state, reply[0], reply[1], reply[2]);
        return false;
    }
    else
        return true;
}

void clearBuffer(Camera_t *cam) {
    int t_count = 0;
    int length = 0;
    int timeout = 2 * TO_SCALE;

    while ((timeout != t_count) && (length != CAMERABUFFSIZ))
    {
        int avail = serialDataAvail(cam->fd);
        if (avail <= 0)
        {
            t_count++;
            continue;
        }
        t_count = 0;
        // there's a byte!
        serialGetchar(cam->fd);
        length++;
    }
}

void reset(Camera_t *cam) {
    // Camera Reset method
    serialPutchar(cam->fd, (char)0x56);
    serialPutchar(cam->fd, (char)cam->serialNum);
    serialPutchar(cam->fd, (char)RESET);
    serialPutchar(cam->fd, (char)0x00);

    if (checkReply(cam, RESET, 5) != true)
    {
        OS_printf("reset() Check Reply Status: %s\n", strerror(errno));
    }
    clearBuffer(cam);
}

void resumeVideo(Camera_t *cam)
{
    serialPutchar(cam->fd, (char)0x56);
    serialPutchar(cam->fd, (char)cam->serialNum);
    serialPutchar(cam->fd, (char)FBUF_CTRL);
    serialPutchar(cam->fd, (char)0x01);
    serialPutchar(cam->fd, (char)RESUMEFRAME);

    if (checkReply(cam, FBUF_CTRL, 5) == false)
        OS_printf("Camera did not resume\n");
}

int getVersion(Camera_t *cam)
{
    //OS_printf("getVersion() called.\n");
    serialPutchar(cam->fd, (char)0x56);
    serialPutchar(cam->fd, (char)cam->serialNum);
    serialPutchar(cam->fd, (char)GEN_VERSION);
    serialPutchar(cam->fd, (char)0x00);

    bool reply;
    if ((reply = checkReply(cam, GEN_VERSION, 5)) == false)
    {
        //OS_printf("CAMERA NOT FOUND!!!\n");
	return -1;
    }
	//OS_printf("VC0706: check Reply returned: %d\n", reply);
    int counter = 0;
    cam->bufferLen = 0;
    int avail = 0;
    int timeout = 1 * TO_SCALE;

    while ((timeout != counter) && (cam->bufferLen != CAMERABUFFSIZ))
    {
        avail = serialDataAvail(cam->fd);
        if (avail <= 0)
        {
            usleep(TO_U);
            counter++;
            continue;
        }
        counter = 0;
        // there's a byte!
        int newChar = serialGetchar(cam->fd);
        cam->camerabuff[cam->bufferLen++] = (char)newChar;
    }

    cam->camerabuff[cam->bufferLen] = 0;
    //OS_printf("VC0706: camera Version: '%s'\n", (char *)cam->camerabuff);
    //OS_printf("getVersion() returning.\n");
    return 0;
}

void setMotionDetect(Camera_t *cam, int flag)
{
    serialPutchar(cam->fd, (char)0x56);
    serialPutchar(cam->fd, (char)0x00);
    serialPutchar(cam->fd, (char)0x42);
    serialPutchar(cam->fd, (char)0x04);
    serialPutchar(cam->fd, (char)0x01);
    serialPutchar(cam->fd, (char)0x01);
    serialPutchar(cam->fd, (char)0x00);
    serialPutchar(cam->fd, (char)0x00);

    serialPutchar(cam->fd, (char)0x56);
    serialPutchar(cam->fd, (char)cam->serialNum);
    serialPutchar(cam->fd, (char)COMM_MOTION_CTRL);
    serialPutchar(cam->fd, (char)0x01);
    serialPutchar(cam->fd, (char)flag);

    clearBuffer(cam);
}

char * takePicture(Camera_t *cam, char * file_path)
{
    cam->frameptr = 0;

    //OS_printf("takePicture() called.\n");

    // Enable LED
    //OS_printf("LED ON\n");
    led_on(&led); // initialized in vc0706_device.c
    OS_TaskDelay(50); // wait one 1ms to allow the LED to heat up

    //Clear Buffer
    clearBuffer(cam);

    serialPutchar(cam->fd, (char)0x56);
    serialPutchar(cam->fd, (char)cam->serialNum);
    serialPutchar(cam->fd, (char)FBUF_CTRL);
    serialPutchar(cam->fd, (char)0x01);
    serialPutchar(cam->fd, (char)STOPCURRENTFRAME); // Hold the current frame -- this essentially takes the picture

    // Disable LED
    led_off(&led);
    //OS_printf("LED OFF\n");

    if (checkReply(cam, FBUF_CTRL, 5) == false)
    {
        OS_printf("Frame checkReply Failed\n");
        return cam->empty;
    }

    //OS_printf("VC0706_core::takePicture() retrieving FBUFF_LEN...\n");

    serialPutchar(cam->fd, (char)0x56);
    serialPutchar(cam->fd, (char)cam->serialNum);
    serialPutchar(cam->fd, (char)GET_FBUF_LEN);
    serialPutchar(cam->fd, (char)0x01);
    serialPutchar(cam->fd, (char)0x00);

    if (checkReply(cam, GET_FBUF_LEN, 5) == false)
    {
        OS_printf("FBUF_LEN REPLY NOT VALID!!!\n");
        return cam->empty;
    }

    while(serialDataAvail(cam->fd) <= 0){;}

    //OS_printf("Serial Data Avail %d \n", serialDataAvail(cam->fd));

    unsigned int len;
    len = serialGetchar(cam->fd);
    len <<= 8;
    len |= serialGetchar(cam->fd);
    len <<= 8;
    len |= serialGetchar(cam->fd);
    len <<= 8;
    len |= serialGetchar(cam->fd);

    //OS_printf("Length %u \n", len);

    if(len > 20000){
        //OS_printf("Camera returned en:%u too Large for camera buffer size. Should be <= 20000 \n", len);
        /*
        CFE_EVS_SendEvent(VC0706_LEN_ERR_EID, CFE_EVS_ERROR, "Image length reported from Camera %d too large for camera buffer. len reported: %u, expected value <= 20000. Attempting to take another image with same name.", mux.mux_state, len);
        */
        CFE_EVS_SendEvent(VC0706_LEN_ERR_EID, CFE_EVS_ERROR, "Camera %d  image too large. Length [%u] Expected <= 20000", mux.mux_state, len);
        resumeVideo(cam);
        clearBuffer(cam);
        return takePicture(cam, file_path);
    }
    //char image[len];
    char * image = malloc(len+1);
    //image[len+1] = NULL;

    int imgIndex = 0;

    while (len > 0)
    {
        unsigned int readBytes = len;

        serialPutchar(cam->fd, (char)0x56);
        serialPutchar(cam->fd, (char)cam->serialNum);
        serialPutchar(cam->fd, (char)READ_FBUF);
        serialPutchar(cam->fd, (char)0x0C);
        serialPutchar(cam->fd, (char)0x0);
        serialPutchar(cam->fd, (char)0x0A);
        serialPutchar(cam->fd, (char)(cam->frameptr >> 24 & 0xff));
        serialPutchar(cam->fd, (char)(cam->frameptr >> 16 & 0xff));
        serialPutchar(cam->fd, (char)(cam->frameptr >> 8 & 0xff));
        serialPutchar(cam->fd, (char)(cam->frameptr & 0xFF));
        serialPutchar(cam->fd, (char)(readBytes >> 24 & 0xff));
        serialPutchar(cam->fd, (char)(readBytes >> 16 & 0xff));
        serialPutchar(cam->fd, (char)(readBytes >> 8 & 0xff));
        serialPutchar(cam->fd, (char)(readBytes & 0xFF));
        serialPutchar(cam->fd, (char)(CAMERADELAY >> 8));
        serialPutchar(cam->fd, (char)(CAMERADELAY & 0xFF));

        if (checkReply(cam, READ_FBUF, 5) == false)
        {
	    OS_printf("VC0706: Error! checkReply(cam, READ_FBUF, 5) returned false.\n");
            return cam->empty;
        }

        int counter = 0;
        cam->bufferLen = 0;
        int avail = 0;
        int timeout = 20 * TO_SCALE;

        while ((timeout != counter) && cam->bufferLen < readBytes)
        {
            avail = serialDataAvail(cam->fd);

            if (avail <= 0)
            {
                usleep(TO_U);
                counter++;
                continue;
            }
            counter = 0;
            int newChar = serialGetchar(cam->fd);

	    image[imgIndex++] = (char)newChar;

            cam->bufferLen++;
        }

        cam->frameptr += readBytes;
        len -= readBytes;

        if (checkReply(cam, READ_FBUF, 5) == false)
        {
            OS_printf("ERROR READING END OF CHUNK| start: %u | length: %u\n", cam->frameptr, len);
        }
    }

    //OS_printf("VC0706_CORE: Attempting to open file...\n");
    // FILE *jpg = fopen(file_path, "w");
    int32 pic_fd = OS_creat(file_path, (int32)OS_READ_WRITE);
    //if (jpg != NULL)
    if(!(pic_fd < OS_FS_SUCCESS)) // if successful file creat
    {	// test
	//int i=0;
	//while(image[i] != '\0')
	//{
	//	i++;
	//}
	//printf("VC0706: Manual determined length of image: %d bytes\n", i);
        //size_t stored = fwrite(image, sizeof(image[0]), imgIndex, jpg);
        //fclose(jpg);
	OS_write(pic_fd, (void *)image, imgIndex);
	OS_close(pic_fd);
	//printf("VC0706: number of stored bytes:%zu\n", stored);
	//if((size_t)i != stored)
	//{
    	//    OS_printf("VC0706 ERROR: image stored %zu bytes, expected to store %d bytes\n", stored, i);
	//}
    }
    else
    {
        CFE_EVS_SendEvent(VC0706_CHILD_INIT_INF_EID, CFE_EVS_ERROR, "IMAGE FILE COULD NOT BE OPENED/MADE!");
        //OS_printf("IMAGE COULD NOT BE OPENED/MADE!\n"); // Should get EVS
	return (char *)NULL;
    }

    //OS_printf("VC0706: copying file_path <%s> of size %d to imageName\n", file_path, strlen(file_path));
    //strcpy(cam->imageName, file_path);
    strncpy(cam->imageName, file_path, strlen(file_path));
    resumeVideo(cam);

    //Clear Buffer
    clearBuffer(cam);

    CFE_EVS_SendEvent(VC0706_CHILD_INIT_INF_EID, CFE_EVS_ERROR, "Camera %d stored as <%s>", mux.mux_state, cam->imageName);
    return cam->imageName;
}

