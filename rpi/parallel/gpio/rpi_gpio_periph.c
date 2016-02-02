/*
http://www.pieter-jan.com/node/15
2013 Pieter-Jan Van de Maele All Rights Reserved
*/
/********************************************************
** Editted by Keegan Moore:
*  Fixed variable scope issues with struct bcm2835_peripheral gpio
*  
*/

#include "rpi_gpio_periph.h"

 
// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int map_peripheral(struct bcm2835_peripheral *p)
{
   // Open /dev/mem
   if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("Failed to open /dev/mem, try checking permissions.\n");
      return -1;
   }
 
   p->map = mmap(
      NULL,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      p->mem_fd,      // File descriptor to physical memory virtual file '/dev/mem'
      p->addr_p       // Address in physical map that we want this memory block to expose
   );
 
   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }
 
   p->addr = (volatile unsigned int *)p->map;
 
   return 0;
}
 
void unmap_peripheral(struct bcm2835_peripheral *p) {
 
    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}
