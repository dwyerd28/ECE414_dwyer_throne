#ifndef LED_OUT_H
#define LED_OUT_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"


void led_out_init(void);

void led_out_write(uint8_t pattern);

#endif 
