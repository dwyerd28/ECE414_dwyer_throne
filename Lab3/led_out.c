

#include "led_out.h"


static const uint8_t LED_PINS[8] = { 2, 3, 4, 5, 6, 7, 8, 9 };

void led_out_init(void) {
    for (int i = 0; i < 8; ++i) {
        gpio_init(LED_PINS[i]);
        gpio_set_dir(LED_PINS[i], true);   
        gpio_put(LED_PINS[i], 0);          
    }
}

void led_out_write(uint8_t pattern) {
    for (int i = 0; i < 8; ++i) {
        
        bool on = (pattern >> i) & 0x1;
        gpio_put(LED_PINS[i], on);
    }
}

