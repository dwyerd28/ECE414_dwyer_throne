

#ifndef DEBOUNCER2_H
#define	DEBOUNCER2_H

#include "pico/stdlib.h"


#define DEBOUNCE_PD_MS 25

void debounce_sw2_init();

void debounce_sw2_tick();


bool debounce_sw2_pressed();

#endif	


