#ifndef _ADS1115_READ_H_
#define _ADS1115_READ_H_

#include <stdio.h>
//#include <stdlib.h>
#include <stdint.h> /* Data type definitions (uint8_t ...) */
//#include <string.h>
//#include <limits.h>
//#include <unistd.h> /* Unix standard function definitions */
#include <fcntl.h> /* File control definitions (O_RDWR ... ) */
//#include <errno.h> /* Error number definitons */
//#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

float ads1115_read(int pin);

#endif
