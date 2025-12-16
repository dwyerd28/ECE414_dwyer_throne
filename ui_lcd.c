
#include "ui_lcd.h"
#include "TFTMaster.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define HEADER_Y       10
#define PERCENT_Y      40
#define PROG_BAR_X     20
#define PROG_BAR_Y     80
#define PROG_BAR_W     200
#define PROG_BAR_H     20

void ui_lcd_init(void) {
    tft_init_hw();
    tft_begin();
    
    tft_setRotation(3);
    tft_fillScreen(ILI9340_BLACK);

    // header text
    tft_setTextWrap(0);
    tft_setTextSize(2);
    tft_setTextColor(ILI9340_WHITE);
    tft_setCursor(10, HEADER_Y);
    tft_writeString("Drawing Progress");

    // percentage label that starts at 0
    tft_setCursor(10, PERCENT_Y);
    tft_writeString("0%");

    // progress bar outline
    tft_drawRect(PROG_BAR_X, PROG_BAR_Y, PROG_BAR_W, PROG_BAR_H, ILI9340_WHITE);
   //when exmpty
    tft_fillRect(PROG_BAR_X + 1, PROG_BAR_Y + 1,
                 PROG_BAR_W - 2, PROG_BAR_H - 2, ILI9340_BLACK);
}

void ui_lcd_update_progress(size_t done, size_t total) {
    float frac = 0.0f;
    if (total > 0) {
        frac = (float)done / (float)total;
        if (frac < 0.0f) frac = 0.0f;
        if (frac > 1.0f) frac = 1.0f;
    }

    //percent text
    char buf[16];
    int percent = (int)(frac * 100.0f + 0.5f);

    // clear previous percent area
    tft_fillRect(10, PERCENT_Y, 80, 16, ILI9340_BLACK);

    tft_setCursor(10, PERCENT_Y);
    tft_setTextSize(2);
    tft_setTextColor(ILI9340_WHITE);
    snprintf(buf, sizeof(buf), "%d%%", percent);
    tft_writeString(buf);

    // update progress bar fill 
    int inner_width = PROG_BAR_W - 2;
    int filled = (int)(frac * inner_width + 0.5f);

   //fills progress bar with green
    tft_fillRect(PROG_BAR_X + 1, PROG_BAR_Y + 1,
                 filled, PROG_BAR_H - 2, ILI9340_GREEN);

    // makes remaining poriton of bar black
    tft_fillRect(PROG_BAR_X + 1 + filled, PROG_BAR_Y + 1,
                 inner_width - filled, PROG_BAR_H - 2, ILI9340_BLACK);
}

void ui_lcd_show_done(void) {
    tft_setTextSize(2);
    tft_setTextColor(ILI9340_YELLOW);
    tft_setCursor(10, PERCENT_Y + 40);
    tft_writeString("Done!");
}


