

#include "stdio.h"
#include "pico/stdlib.h"
#include "debounce_sw2.h"
#include "sw_in.h"


static bool btn2_pressed;  

typedef enum { NOPUSH2, MAYBEPUSH2, PUSHED2, MAYBENOPUSH2 } DB2_States;
static DB2_States DB2_State;

void debounce_sw2_init() {
    DB2_State = NOPUSH2;
    btn2_pressed = false;
}

void debounce_sw2_tick() {
    bool btn = sw_in_read2();
    switch(DB2_State) {
        case NOPUSH2:
            if (btn) DB2_State = MAYBEPUSH2;
            else DB2_State = NOPUSH2;
            break;
        case MAYBEPUSH2:
            if (btn) {
                btn2_pressed = true;
                DB2_State = PUSHED2;
            }
            else DB2_State = NOPUSH2;
            break;
        case PUSHED2:
            if (btn) DB2_State = PUSHED2;
            else DB2_State = MAYBENOPUSH2;
            break;
        case MAYBENOPUSH2:
            if (btn) DB2_State = PUSHED2;
            else DB2_State = NOPUSH2;
            break;
        default:
            DB2_State = NOPUSH2;
            break;
    }
}


bool debounce_sw2_pressed() {
    if (btn2_pressed) {
        btn2_pressed = false; 
        return true;
    } else return false;
}


