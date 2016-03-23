/*******************************************************************************
** File: vc0706_mux.h
**
** Purpose:
**   This file is main hdr file for the VC0706 application.
**
**
*******************************************************************************/

#ifndef	_vc0706_mux_h_
#define _vc0706_mux_h_

#include "vc0706.h"


typedef struct mux_t { // Not sure how much should be implimented in this struct
	int mux_select_pin; // Expects GPIO for mux select line
	int mux_state; 		// should always be 0 or 1
} mux_t;


int mux_init(mux_t *mux, int select_pin);

int mux_select(mux_t *mux, int select); // sanatized selection.

int mux_switch(mux_t *mux); // swap selection

#endif /* _vc0706_mux_h_ */
