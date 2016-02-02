/*
http://www.pieter-jan.com/node/15
2013 Pieter-Jan Van de Maele All Rights Reserved
*/
#include "rpi_gpio_periph.h"
 
extern struct bcm2835_peripheral gpio = {GPIO_BASE};

/*
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
*/

int main(int argc, char* argv[])
{
    if(map_peripheral(&gpio) == -1) 
    {
        printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
        unmap_peripheral(&gpio);
        return -1;
    }
    
    int gpio_pin = 0;
    
    for(gpio_pin=24; gpio_pin <= 45; gpio_pin++)
    {
        INP_GPIO(gpio_pin);
        OUT_GPIO(gpio_pin);
    }    

    /*
    ** Infinite Flash Loop
    */
    while(1)
    {
        for(gpio_pin=24; gpio_pin <= 31; gpio_pin++)
        {
            GPIO_SET0 |= 1 << (gpio_pin);
        }
        for(gpio_pin=32; gpio_pin <= 45; gpio_pin++)
        {
            GPIO_SET1 |= 1 << (gpio_pin - 32);
        }
        
        sleep(3);
        
        for(gpio_pin=24; gpio_pin < 32; gpio_pin++)
        {
            GPIO_CLR0 |= 1 << (gpio_pin);
        }
        for(gpio_pin=32; gpio_pin < 45; gpio_pin++)
        {
            GPIO_CLR1 |= 1 << (gpio_pin -32);
        }
        
        sleep(3);
    }
    
    unmap_peripheral(&gpio);
    
	return 0;
}
