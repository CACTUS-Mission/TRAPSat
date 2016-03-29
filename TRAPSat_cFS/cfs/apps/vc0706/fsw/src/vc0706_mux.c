/*******************************************************************************
** File: vc0706_mux.c
**
** Purpose:
**   This file is main hdr file for the VC0706 application.
**
**
*******************************************************************************/

#include "vc0706_mux.h"

int mux_init(mux_t *mux, int select_pin) {
	mux->mux_select_pin = select_pin;
	pinMode(select_pin, OUTPUT);
	int ret = mux_select(mux, 0); // initialize low
	return ret;
}

int mux_select(mux_t *mux, int select) { 
	if(select == 1)
	{
		digitalWrite(mux->mux_select_pin, HIGH);
		mux->mux_state = 1;
		return 0;
	}
	else if(select == 0)
	{
		digitalWrite(mux->mux_select_pin, LOW);
		mux->mux_state = 0;
		return 0;
	}
	else
	{
		return -1;
	}
}


int mux_switch(mux_t *mux) { // swap selection
	if(mux->mux_state == 0)
		return mux_select(mux, 1);
	else
		return mux_select(mux, 0);
}
