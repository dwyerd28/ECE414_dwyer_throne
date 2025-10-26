#include "TouchDisplay.h"
#include "TFTMaster.h"
#include "ts_lcd.h"
#include <stdio.h>


#define DISP_X       10
#define DISP_Y       10
#define DISP_W       250
#define DISP_H       40


#define BTN_W        50
#define BTN_H        36
#define BTN_SP       10
#define COL1_X       10
#define COL2_X       (COL1_X + BTN_W + BTN_SP)           // 83
#define COL3_X       (COL2_X + BTN_W + BTN_SP)           // 151
#define COL4_X       (COL3_X + BTN_W + BTN_SP)           // 219
#define ROW1_Y       60
#define ROW2_Y       (ROW1_Y + BTN_H + BTN_SP)           // 104
#define ROW3_Y       (ROW2_Y + BTN_H + BTN_SP)           // 148
#define ROW4_Y       (ROW3_Y + BTN_H + BTN_SP)           // 192

static inline void draw_btn(uint16_t x, uint16_t y, const char *label) {
    tft_fillRect(x, y, BTN_W, BTN_H, ILI9340_BLUE);
    tft_drawRect(x, y, BTN_W, BTN_H, ILI9340_WHITE);
    // Center-ish text
    tft_setCursor(x + (BTN_W/2) - 6, y + (BTN_H/2) - 6);
    tft_setTextColor(ILI9340_WHITE);
    tft_writeString((char *)label);
}

void drawCalcScreen() {
    tft_fillScreen(ILI9340_BLACK);
    tft_setTextSize(2);

    // Display area (use supported colors only)
    tft_fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_WHITE);
    tft_drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_BLUE);
    tft_setCursor(DISP_X + 5, DISP_Y + 12);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString("0");

    
    // Row1: 7 8 9 +
    draw_btn(COL1_X, ROW1_Y, "7");
    draw_btn(COL2_X, ROW1_Y, "8");
    draw_btn(COL3_X, ROW1_Y, "9");
    draw_btn(COL4_X, ROW1_Y, "+");

    // Row2: 4 5 6 -
    draw_btn(COL1_X, ROW2_Y, "4");
    draw_btn(COL2_X, ROW2_Y, "5");
    draw_btn(COL3_X, ROW2_Y, "6");
    draw_btn(COL4_X, ROW2_Y, "-");

    // Row3: 1 2 3 *
    draw_btn(COL1_X, ROW3_Y, "1");
    draw_btn(COL2_X, ROW3_Y, "2");
    draw_btn(COL3_X, ROW3_Y, "3");
    draw_btn(COL4_X, ROW3_Y, "*");

    // Row4: C 0 = /
    draw_btn(COL1_X, ROW4_Y, "C");
    draw_btn(COL2_X, ROW4_Y, "0");
    draw_btn(COL3_X, ROW4_Y, "=");
    draw_btn(COL4_X, ROW4_Y, "/");
}

static inline bool in_box(uint16_t x, uint16_t y, uint16_t bx, uint16_t by) {
    return (x >= bx) && (x < (bx + BTN_W)) && (y >= by) && (y < (by + BTN_H));
}

Buttons getButton(uint16_t x, uint16_t y) {
    // Row1
    if (in_box(x,y,COL1_X,ROW1_Y)) return B7;
    if (in_box(x,y,COL2_X,ROW1_Y)) return B8;
    if (in_box(x,y,COL3_X,ROW1_Y)) return B9;
    if (in_box(x,y,COL4_X,ROW1_Y)) return BPLUS;

    // Row2
    if (in_box(x,y,COL1_X,ROW2_Y)) return B4;
    if (in_box(x,y,COL2_X,ROW2_Y)) return B5;
    if (in_box(x,y,COL3_X,ROW2_Y)) return B6;
    if (in_box(x,y,COL4_X,ROW2_Y)) return BMINUS;

    // Row3
    if (in_box(x,y,COL1_X,ROW3_Y)) return B1;
    if (in_box(x,y,COL2_X,ROW3_Y)) return B2;
    if (in_box(x,y,COL3_X,ROW3_Y)) return B3;
    if (in_box(x,y,COL4_X,ROW3_Y)) return BMULT;

    // Row4
    if (in_box(x,y,COL1_X,ROW4_Y)) return BCL;
    if (in_box(x,y,COL2_X,ROW4_Y)) return B0;
    if (in_box(x,y,COL3_X,ROW4_Y)) return BEQ;
    if (in_box(x,y,COL4_X,ROW4_Y)) return BDIV;

    return BERR;
}

void displayResult(int32_t num) {
    // Clear display area
    tft_fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_WHITE);
    tft_setCursor(DISP_X + 5, DISP_Y + 12);
    tft_setTextColor(ILI9340_BLACK);
    char buf[20];
    snprintf(buf, sizeof(buf), "%ld", (long)num);
    tft_writeString(buf);
}

void displayError() {
    tft_fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_WHITE);
    tft_setCursor(DISP_X + 5, DISP_Y + 12);
    tft_setTextColor(ILI9340_RED); //ILI9340_WHITE);
    tft_writeString("ERROR");
}

void displayDiv0() {
    tft_fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, ILI9340_WHITE);
    tft_setCursor(DISP_X + 5, DISP_Y + 12);
    tft_setTextColor(ILI9340_RED); //ILI9340_WHITE);
    tft_writeString("DIV0");
}


