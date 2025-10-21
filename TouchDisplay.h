#ifndef TOUCHDISPLAY_H
#define TOUCHDISPLAY_H
#include "pico/stdlib.h"
#include "stdbool.h"
#include "inttypes.h"

// Calculator buttons laid out in a 4x4 grid
typedef enum Buttons {
    B0, B1, B2, B3, B4, B5, B6, B7, B8, B9,   // digits
    BPLUS, BMINUS, BMULT, BDIV,             // + - * /
    BEQ, BCL,                                // =  C
    BERR                                     // invalid touch / none
} Buttons;

// Public UI functions
void drawCalcScreen(void);
Buttons getButton(uint16_t x, uint16_t y);
void displayResult(int32_t num);
void displayError(void);
void displayDiv0(void);

#endif


