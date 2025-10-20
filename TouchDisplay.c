#include "TouchDisplay.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include <stdio.h>
#include <string.h>


#define BUTTON_WIDTH 70
#define BUTTON_HEIGHT 50
#define COL1_X 20
#define COL2_X 100
#define COL3_X 180
#define COL4_X 260
#define ROW1_Y 60
#define ROW2_Y 120
#define ROW3_Y 180
#define ROW4_Y 240

static char displayBuffer[20];

static inline void drawButton(int x, int y, const char* label, uint16_t color){
    tft_fillRect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, color);
    tft_setCursor(x + 25, y + 15);
    tft_setTextColor(ILI9340_WHITE);
    tft_setTextSize(2);
    tft_writeString(label);
    tft_drawRect(x, y, BUTTON_WIDTH, BUTTON_HEIGHT, ILI9340_WHITE);
}

void drawCalcScreen() {
    tft_fillScreen(ILI9340_BLACK);
    tft_setTextSize(2);

    // Display area
    tft_fillRect(10, 10, 300, 40, ILI9340_DARKGREY);

    // Draw number buttons 1-9 in 3x3 grid
    drawButton(COL1_X, ROW1_Y, "1", ILI9340_BLUE);
    drawButton(COL2_X, ROW1_Y, "2", ILI9340_BLUE);
    drawButton(COL3_X, ROW1_Y, "3", ILI9340_BLUE);

    drawButton(COL1_X, ROW2_Y, "4", ILI9340_BLUE);
    drawButton(COL2_X, ROW2_Y, "5", ILI9340_BLUE);
    drawButton(COL3_X, ROW2_Y, "6", ILI9340_BLUE);

    drawButton(COL1_X, ROW3_Y, "7", ILI9340_BLUE);
    drawButton(COL2_X, ROW3_Y, "8", ILI9340_BLUE);
    drawButton(COL3_X, ROW3_Y, "9", ILI9340_BLUE);

    // Button 0, C, = in bottom row
    drawButton(COL1_X, ROW4_Y, "0", ILI9340_BLUE);
    drawButton(COL2_X, ROW4_Y, "C", ILI9340_ORANGE);
    drawButton(COL3_X, ROW4_Y, "=", ILI9340_ORANGE);

    // Operator buttons in right column
    drawButton(COL4_X, ROW1_Y, "+", ILI9340_RED);
    drawButton(COL4_X, ROW2_Y, "-", ILI9340_RED);
    drawButton(COL4_X, ROW3_Y, "*", ILI9340_RED);
    drawButton(COL4_X, ROW4_Y, "/", ILI9340_RED);
}

static inline bool inRect(uint16_t x, uint16_t y, int rx, int ry){
    return (x >= rx && x <= rx + BUTTON_WIDTH && y >= ry && y <= ry + BUTTON_HEIGHT);
}

Buttons getButton(uint16_t x, uint16_t y) {
    // Simple button detection for all 16 buttons
    if(inRect(x,y,COL1_X,ROW1_Y)) return B1;
    if(inRect(x,y,COL2_X,ROW1_Y)) return B2;
    if(inRect(x,y,COL3_X,ROW1_Y)) return B3;

    if(inRect(x,y,COL1_X,ROW2_Y)) return B4;
    if(inRect(x,y,COL2_X,ROW2_Y)) return B5;
    if(inRect(x,y,COL3_X,ROW2_Y)) return B6;

    if(inRect(x,y,COL1_X,ROW3_Y)) return B7;
    if(inRect(x,y,COL2_X,ROW3_Y)) return B8;
    if(inRect(x,y,COL3_X,ROW3_Y)) return B9;

    if(inRect(x,y,COL1_X,ROW4_Y)) return B0;
    if(inRect(x,y,COL2_X,ROW4_Y)) return BCL;
    if(inRect(x,y,COL3_X,ROW4_Y)) return BEQ;

    if(inRect(x,y,COL4_X,ROW1_Y)) return BPLUS;
    if(inRect(x,y,COL4_X,ROW2_Y)) return BMINUS;
    if(inRect(x,y,COL4_X,ROW3_Y)) return BMULT;
    if(inRect(x,y,COL4_X,ROW4_Y)) return BDIV;

    return BERR;
}

void displayResult(int32_t num) {
    tft_fillRect(10, 10, 300, 40, ILI9340_DARKGREY);
    tft_setCursor(15, 20);
    tft_setTextColor(ILI9340_WHITE, ILI9340_DARKGREY);
    sprintf(displayBuffer, "%ld", num);
    tft_writeString(displayBuffer);
}

void displayError() {
    tft_fillRect(10, 10, 300, 40, ILI9340_DARKGREY);
    tft_setCursor(15, 20);
    tft_setTextColor(ILI9340_RED, ILI9340_DARKGREY);
    tft_writeString("ERROR");
}

void displayDiv0(){
    tft_fillRect(10, 10, 300, 40, ILI9340_DARKGREY);
    tft_setCursor(15, 20);
    tft_setTextColor(ILI9340_RED, ILI9340_DARKGREY);
    tft_writeString("DIV0");
}


