/*
http://www.pieter-jan.com/node/15
2013 Pieter-Jan Van de Maele All Rights Reserved
*/
#include "rpi_gpio_periph.h"

int main(int argc, char* argv[])
{
    if(map_peripheral(&gpio) == -1) 
    {
        printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
        unmap_peripheral(&gpio);
        return -1;
    }

    INP_GPIO(4);
    OUT_GPIO(4);
    
    INP_GPIO(35);
    OUT_GPIO(35);

    while(1)
    {
        // Toggle pin 7 (blink a led!)
        GPIO_SET0 = 1 << 4;
        //GPIO_SET0 = 1 << 35;
        GPIO_SET1 = 1 << 4;
        sleep(3);
        
        //GPIO_CLR0 = 1 << 35;
        GPIO_CLR0 = 1 << 4;
        GPIO_CLR1 = 1 << 4;
        sleep(3);
    }
    
    unmap_peripheral(&gpio);
    
	return 0;
}
