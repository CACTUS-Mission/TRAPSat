/*
http://www.pieter-jan.com/node/15
2013 Pieter-Jan Van de Maele All Rights Reserved
*********************************************************
** Editted by Keegan Moore:
*  Included FCNTL.H to solve dependency issues
*  Editted GP SET/CLR macros
*/

#ifndef _RPI_GPIO_PERIPH_H_
#define _RPI_GPIO_PERIPH_H_


#include <stdio.h> 
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
 
#include <fcntl.h>

#define BCM2708_PERI_BASE       0x20000000
#define GPIO_BASE               (BCM2708_PERI_BASE + 0x200000)	// GPIO controller 
 
#define BLOCK_SIZE 		(4*1024)
 
// IO Acces
struct bcm2835_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};

/**/
struct bcm2835_peripheral gpio;  // They have to be found somewhere, but can't be in the header

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x)
#define INP_GPIO(g)   *(gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)   *(gpio.addr + ((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio.addr + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3))
#define GPIO_SET0  *(gpio.addr + 7)  // sets pins 0 --> 31  bits which are 1 ignores bits which are 0
#define GPIO_SET1  *(gpio.addr + 8)  // sets pins 32 --> 53  bits which are 1 ignores bits which are 0
#define GPIO_CLR0  *(gpio.addr + 10) // clears pins 0 --> 31 bits which are 1 ignores bits which are 0
#define GPIO_CLR1  *(gpio.addr + 11) // clears pins 32 --> 53bits which are 1 ignores bits which are 0
#define GPIO_SET(g) (((g) >= 32) && ((g) <= 53)) ? *(gpio.addr + 8) : ((g) >= 0) ?  *(gpio.addr + 7) : NULL;
#define GPIO_READ(g)  *(gpio.addr + 13) &= (1<<(g))
/**/

#endif /*Definition of _RPI_GPIO_PERIPH_H_  */
