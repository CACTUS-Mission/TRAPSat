#include "tim_serial_out.h"

extern serial_out_t TIM_SerialUSB;

int serial_out_init(serial_out_t *serial, char * port) // opens the serial port and sets it to serial->fd
{
    serial->fd = serialOpen(port, SERIAL_OUT_BAUD);
    serial->data = 0x00;
    if(serial->fd == -1)
    {
        printf("serial_out_init(): ERROR: %s", strerror(errno));
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
/*
int serial_write_file(serial_out_t *serial, char* file_path)
{
    FILE *file_to_write;
    unsigned char data_buff;
    file_to_write = fopen(file_path, "r");

    if(file_to_write == NULL)
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
*/
int tim_serial_write_file(serial_out_t *serial, char* file_path)
{
    int os_fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    const uint32 bytes_per_read = 1; /* const */
    uint16 total_bytes_read = 0;
    
    uint8 data_buf[bytes_per_read];
    int i;

    for(i = 0; i < bytes_per_read; i++) 
    {
        data_buf[i] = 0;
    }

    //OS_printf("Size of file_path: [%d]\n", sizeof(file_path));
    OS_printf("Calling OS_open(\'%s\', OS_READ_ONLY, mode);\n", file_path);

    /*
    ** Open File with Read Only permissions
    */
    if ((os_fd = OS_open((const char * ) file_path, (int32) OS_READ_ONLY, (uint32) mode)) < OS_FS_SUCCESS)
    {
        OS_printf("TIM: OS_open Returned [%d] (expected non-negative value).\n", os_fd);
        return -1;
    }

    /*
    ** Read 1 byte at a time from file, write to serial.
    */
    while( OS_read((int32) os_fd, (void *) data_buf, (uint32) bytes_per_read))
    {
        //OS_printf("From serial: File '%s' Byte %.4d = %#.2X\n", file_path, total_bytes_read, data_buf[0]);
        total_bytes_read++;
        serial_write_byte(&TIM_SerialUSB, (unsigned char) data_buf[0]);
        data_buf[0] = 0;
    }

    /*
    ** Close the file when finished reading data
    */
    OS_close(os_fd);

    return total_bytes_read;
}

int serial_out_close(serial_out_t *serial) // opens the serial port and sets it to serial->fd
{
    serialClose(serial->fd);
    return 0;
}