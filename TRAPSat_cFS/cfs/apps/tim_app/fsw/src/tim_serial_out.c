#include "tim_serial_out.h"

int serial_out_init(serial_out_t *serial, char * port) // opens the serial port and sets it to serial->fd
{
    serial->fd = serialOpen(port, BAUD);
    serial->data = 0x00;
    if(serial->fd == -1)
    {
        printf("serial_out: ERROR: %s", strerror(errno));
        return -1;
    }
    else
    {
        return 0;
    }
}

void serial_write_byte(serial_out_t *serial, unsigned char byte) // writes byte to serial port
{
    serialPutchar(serial->fd, byte);
    serial->data = byte;
}

int serial_write_file(serial_out_t *serial, char* file_path)
{
    FILE *file_to_write;
    unsigned char data_buff;
    file_to_write = fopen(file_path, "r");

    if(file_to_write = NULL)
    {
        printf("Error Opening file: %s\n", file_path);
        return -1;
    }
    while(1) // maybe add timeout?
    {
        data_buff = (unsigned char) fgetc(file_to_write); // returns int, but is really an unsigned char
        if(feof(file_to_write))
        {
            break; // end of file
        }
        serial_write_byte(serial, data_buff); // output to serial
    }
    fclose(file_to_write);
    return 0;
}
