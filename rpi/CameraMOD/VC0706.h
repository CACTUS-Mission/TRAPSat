/*
 * VC0706.h
 *
 * This is a rewrite of the VC0706 C++ library from vyykn
 * This implimentation is written in C, with no object orientation with the goal of faster image retrieval.
 */
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include <wiringSerial.h>



#define BAUD  38400
#define PORT  "/dev/ttyAMA0" // this is the port on the Raspberry Pi; it will be different for serial ports on other systems.
#define TIMEOUT  0.5    // I needed a longer timeout than ladyada's 0.2 value -- could be tweaked
#define SERIALNUM  0    // start with 0, each camera should have a unique ID.
#define COMMANDSEND  0x56
#define COMMANDREPLY  0x76
#define COMMANDEND  0x00
#define CMD_GETVERSION  0x11
#define CMD_RESET  0x26
#define CMD_TAKEPHOTO  0x36
#define CMD_READBUFF  0x32
#define CMD_GETBUFFLEN  0x34
#define FBUF_CURRENTFRAME  0x00
#define FBUF_NEXTFRAME  0x01
#define FBUF_STOPCURRENTFRAME  0x00
#define SLEEP_TIME 500000


typedef struct VC0706 {
	int fd; 	// fd is a standard variable for SPI devices, it specifies the serial device itself
	bool ready;	// Just as it seems, high when ready, low when not ready
}

VC0706 cam; // maybe this shouldn't be a global, another way to use less memory? consult Keegan.

bool init()
{
	if ( cam.fd=serialOpen(PORT, BAUD) < 0) // open Serial port (GPIO 14(RPi Tx) and 15(RPi Rx)) 
		fprintf(stderr, "SPI Setup Failed: ", strerror (errno));
	
	if(wiringPiSetup() == -1)
		exit(1);

	cam.ready = true;
	return true;
}

bool check_reply(int CMD, int size)
{
    int reply[size];
    int t_count = 0;
    int length = 0;
    int avail = 0;
    int timeout = 0.5;

    while ((timeout != t_count) && (length != CAMERABUFFSIZ) && length < size)
    {
        avail = serialDataAvail(cam.fd);
        if (avail <= 0)
        {
            usleep(SLEEP_TIME);
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

bool reset()
{
	serialPutchar(cam.fd, (char)COMMANDSEND);	// command begin
    serialPutchar(cam.fd, (char)SERIALNUM);		// to camera <serial number>
    serialPutchar(cam.fd, (char)CMD_RESET);		// RESET()
    serialPutchar(cam.fd, (char)COMMANDEND);	// command end 

    if (checkReply(CMD_RESET, 5) != true)
        fprintf(stderr, "Check Reply Status: ", strerror (errno));

    //clearBuffer(); -- not in python code, ommitting
}

bool get_version()
{

}

bool take_photo()
{

}

int get_buffer_length()
{

}

int read_buffer(int bytes)
{

}

bool get_photo(char* file_name)
{

}
