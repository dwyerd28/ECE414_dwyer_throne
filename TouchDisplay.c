#include "TouchDisplay.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include <stdio.h>
#include <string.h>


#define SCREEN_W 320
#define SCREEN_H 240


#define DISP_X 10
#define DISP_Y 10
#define DISP_W 300
#define DISP_H 40


#define BUTTON_W 60
#define BUTTON_H 40
#define GAP_X    10
#define GAP_Y    8


#define COL1_X  20
#define COL2_X  90 
#define COL3_X  160
#define COL4_X  230


#define ROW1_Y  50
#define ROW2_Y  98  
#define ROW3_Y  146    
#define ROW4_Y  194 



static char displayBuffer[20];


static void drawBtn(short x, short y, const char *label, unsigned short fill, unsigned short fg) {
    tft_fillRect(x, y, BUTTON_W, BUTTON_H, fill);
    tft_drawRect(x, y, BUTTON_W, BUTTON_H, ILI9340_WHITE);
    tft_setTextColor2(fg, fill);
    tft_setTextSize(2);

    
    int len = (int)strlen(label);
    short text_w = (short)(len * 12);
    short text_h = 16;
    short tx = x + (BUTTON_W - text_w) / 2;
    short ty = y + (BUTTON_H - text_h) / 2;
    if (tx < x + 2) tx = x + 2;
    if (ty < y + 2) ty = y + 2;

    tft_setCursor(tx, ty);
    tft_writeString((char*)label);
}

void drawCalcScreen(void) {
    tft_fillScreen(ILI9340_BLACK);
    tft_setTextWrap(0);

   
    tft_fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_DARKGREY);
    tft_setCursor(DISP_X + 5, DISP_Y + 12);
    tft_setTextColor2(ILI9340_WHITE, ILI9340_DARKGREY);
    tft_setTextSize(2);
    tft_writeString("0");

    
    drawBtn(COL1_X, ROW1_Y, "7", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL2_X, ROW1_Y, "8", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL3_X, ROW1_Y, "9", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL4_X, ROW1_Y, "/", ILI9340_MAGENTA, ILI9340_WHITE);

    
    drawBtn(COL1_X, ROW2_Y, "4", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL2_X, ROW2_Y, "5", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL3_X, ROW2_Y, "6", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL4_X, ROW2_Y, "*", ILI9340_MAGENTA, ILI9340_WHITE);

    
    drawBtn(COL1_X, ROW3_Y, "1", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL2_X, ROW3_Y, "2", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL3_X, ROW3_Y, "3", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL4_X, ROW3_Y, "-", ILI9340_MAGENTA, ILI9340_WHITE);


    drawBtn(COL1_X, ROW4_Y, "0", ILI9340_BLUE, ILI9340_WHITE);
    drawBtn(COL2_X, ROW4_Y, "C", ILI9340_RED,  ILI9340_WHITE);
    drawBtn(COL3_X, ROW4_Y, "=", ILI9340_GREEN,ILI9340_WHITE);
    drawBtn(COL4_X, ROW4_Y, "+", ILI9340_MAGENTA, ILI9340_WHITE);
}

static inline bool inRect(uint16_t x, uint16_t y, short rx, short ry) {
    return (x >= rx) && (x <= rx + BUTTON_W) && (y >= ry) && (y <= ry + BUTTON_H);
}

Buttons getButton(uint16_t x, uint16_t y) {
    // Row1
    if (inRect(x,y,COL1_X,ROW1_Y)) return B7;
    if (inRect(x,y,COL2_X,ROW1_Y)) return B8;
    if (inRect(x,y,COL3_X,ROW1_Y)) return B9;
    if (inRect(x,y,COL4_X,ROW1_Y)) return BDIV;

    // Row2
    if (inRect(x,y,COL1_X,ROW2_Y)) return B4;
    if (inRect(x,y,COL2_X,ROW2_Y)) return B5;
    if (inRect(x,y,COL3_X,ROW2_Y)) return B6;
    if (inRect(x,y,COL4_X,ROW2_Y)) return BMULT;

    // Row3
    if (inRect(x,y,COL1_X,ROW3_Y)) return B1;
    if (inRect(x,y,COL2_X,ROW3_Y)) return B2;
    if (inRect(x,y,COL3_X,ROW3_Y)) return B3;
    if (inRect(x,y,COL4_X,ROW3_Y)) return BMINUS;

    // Row4
    if (inRect(x,y,COL1_X,ROW4_Y)) return B0;
    if (inRect(x,y,COL2_X,ROW4_Y)) return BCL;
    if (inRect(x,y,COL3_X,ROW4_Y)) return BEQ;
    if (inRect(x,y,COL4_X,ROW4_Y)) return BPLUS;

    return BERR;
}

void displayResult(int32_t num) {
    tft_fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_DARKGREY);
    tft_setCursor(DISP_X + 5, DISP_Y + 12);
    tft_setTextColor2(ILI9340_WHITE, ILI9340_DARKGREY);
    tft_setTextSize(2);
    sprintf(displayBuffer, "%ld", (long)num);
    tft_writeString(displayBuffer);
}

void displayError(void) {
    tft_fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_DARKGREY);
    tft_setCursor(DISP_X + 5, DISP_Y + 12);
    tft_setTextColor2(ILI9340_RED, ILI9340_DARKGREY);
    tft_setTextSize(2);
    tft_writeString("ERROR");
}




