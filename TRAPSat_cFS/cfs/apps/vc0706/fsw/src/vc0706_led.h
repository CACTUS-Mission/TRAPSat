/*******************************************************************************
** File: vc0706_led.h
**
** Purpose:
**   This file contains functions to enable, disable, and toggle the LED for the VC0706 application.
**
**
*******************************************************************************/

// Requires WiringPi
// #include <wiringPi.h>

#ifndef _vc0706_led_h_
#define _vc0706_led_h_

#include "vc0706.h"


typedef struct led_t {
	int led_pin; 	/* Expects a GPIO pin value for the LEDs */
	int led_status;	/* Hold the status of the led, 0 is off 1 is on */
} led_t;


void led_init(led_t *led, int pin);

void led_on(led_t *led);

void led_off(led_t *led);


#endif /* _vc0706_flash_h_ */
