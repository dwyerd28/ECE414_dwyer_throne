#ifndef TOUCHDISPLAY_H
#define TOUCHDISPLAY_H
#include "pico/stdlib.h"
#include "stdbool.h"
#include "inttypes.h"


typedef enum Buttons {
    B0, B1, B2, B3, B4, B5, B6, B7, B8, B9,   
    BPLUS, BMINUS, BMULT, BDIV,              
    BEQ, BCL,                                
    BERR                                      
} Buttons;


void drawCalcScreen(void);
Buttons getButton(uint16_t x, uint16_t y);
void displayResult(int32_t num);
void displayError(void);
void displayDiv0(void);

#endif


