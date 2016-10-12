/*
**
** Test file for the serial_out.h library
**
** compile with:
**	
**
*/

#include "serial_out.h"

#define PORT "/dev/ttyUSB0"

int main(void)
{
	serial_out_t serial_usb;

	if(serial_out_init(&serial_usb, (char *)PORT) == -1) // Error opening port
	{
		return -1;
	}
	int i;
	for(i=0; i<10; i++) // infinite loop
	{
		serial_write_byte(&serial_usb, 0xFF); // write all 1s
		printf("serial_usb.data: %x\n", serial_usb.data);
	}


	return 0;
}
gcc serial_out.c serial_test.c -o serial_test -lwiringPi
