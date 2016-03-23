/*******************************************************************************
** File: vc0706_led.c
**
** Purpose:
**   This file contains functions to enable, disable, and toggle the LED for the VC0706 application.
**
**
*******************************************************************************/

// Requires WiringPi
// #include <wiringPi.h>

#include "vc0706_led.h"

void led_init(led_t *led, int pin) { 
	led->led_pin=pin; 
	pinMode(pin, OUTPUT); 
	led_off(led);
}

void led_on(led_t *led) { 
	digitalWrite(led->led_pin, HIGH); 
	led->led_status=1; 
}

void led_off(led_t *led) { 
	digitalWrite(led->led_pin, LOW); 
	led->led_status=0; 
}
