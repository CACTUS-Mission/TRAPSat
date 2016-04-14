/*************************************************************
**  serial_out.h
**
**  The purpose of this library is to encapsulate the
**  Serial output device, and provide a write function.
**
**  NOTE: This library is currently pointless, as one
**  could simply call the serial functions themselves,
**  but this library may become more useful for keeping
**  track of what has been sent, or sanitizing the data.
**
**************************************************************/

#ifndef _serial_out_h_
#define _serial_out_h_

#define SERIAL_OUT_BAUD 19200

#define SERIAL_OUT_PORT "/dev/ttyUSB0"

/* #define SERIAL_OUT_PORT "/dev/ttyAMA0" */

#include <stdio.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <errno.h>

#include "tim_app.h"

typedef struct serial_out_t {
    int fd;
    unsigned char data; //copy of last sent data -- maybe useful
} serial_out_t;

int serial_out_init(serial_out_t *serial, char * port); // opens the serial port and sets it to serial->fd
void serial_write_byte(serial_out_t *serial, unsigned char byte); // writes byte to serial port
//int serial_write_file(serial_out_t *serial, char* file_path);
int tim_serial_write_file(serial_out_t *serial, char* file_path);
int serial_out_close(serial_out_t *serial);

#endif /*_serial_out_h_*/
