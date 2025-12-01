

#include "lcd_progress.h"

#include <stdio.h>          
#include "pico/stdlib.h"
#include "TFTMaster.h"      
#include "ts_lcd.h"         

//color
#define COLOR_BG        ILI9340_BLACK
#define COLOR_TEXT      ILI9340_WHITE
#define COLOR_BAR_OUT   ILI9340_WHITE
#define COLOR_BAR_EMPTY ILI9340_BLACK
#define COLOR_BAR_FILL  ILI9340_BLUE   


#define HEADER_X        10
#define HEADER_Y        10

#define PERCENT_X       10
#define PERCENT_Y       40

#define BAR_X           10
#define BAR_Y           80
#define BAR_WIDTH       300
#define BAR_HEIGHT      30
#define BAR_BORDER      2

static int inner_bar_width  = 0;
static int inner_bar_height = 0;

// Remember the last percent so we can avoid redrawing unchanged text
static int last_percent = -1;

// LCD display
static void draw_static_ui(void) {
    
    tft_fillScreen(COLOR_BG);
    tft_setTextSize(2);

    tft_setCursor(HEADER_X, HEADER_Y);
    tft_setTextColor(COLOR_TEXT, COLOR_BG);
    tft_writeString("Drawing Progress");

    tft_setCursor(PERCENT_X, PERCENT_Y);
    tft_setTextColor(COLOR_TEXT, COLOR_BG);
    tft_writeString("0% complete");

    
    tft_drawRect(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, COLOR_BAR_OUT);

    inner_bar_width  = BAR_WIDTH  - 2 * BAR_BORDER;
    inner_bar_height = BAR_HEIGHT - 2 * BAR_BORDER;

    tft_fillRect(BAR_X + BAR_BORDER,
                 BAR_Y + BAR_BORDER,
                 inner_bar_width,
                 inner_bar_height,
                 COLOR_BAR_EMPTY);
}

void lcd_progress_init(size_t total_points) {
    (void) total_points; 
    ts_lcd_init();
    draw_static_ui();

    last_percent = -1;
}

void lcd_progress_update(size_t current_index, size_t total_points) {
    
    float fraction = 0.0f;
    if (total_points > 0) {
        fraction = (float) current_index / (float) total_points;
    }
    if (fraction < 0.0f) {
        fraction = 0.0f;
    }
    if (fraction > 1.0f) {
        fraction = 1.0f;
    }

    int percent = (int)(fraction * 100.0f + 0.5f);

    // Update Percentage
    if (percent != last_percent) {
        char buf[24];

        
        tft_fillRect(PERCENT_X,
                     PERCENT_Y,
                     160,
                     16,
                     COLOR_BG);

        sprintf(buf, "%d%% complete", percent);
        tft_setCursor(PERCENT_X, PERCENT_Y);
        tft_setTextColor(COLOR_TEXT, COLOR_BG);
        tft_writeString(buf);

        last_percent = percent;
    }

    // Fill in the percentage bar
    int new_fill_width = (int)(fraction * (float)inner_bar_width + 0.5f);

    tft_fillRect(BAR_X + BAR_BORDER,
                 BAR_Y + BAR_BORDER,
                 inner_bar_width,
                 inner_bar_height,
                 COLOR_BAR_EMPTY);

    // Filling the percentage bar left to right
    if (new_fill_width > 0) {
        tft_fillRect(BAR_X + BAR_BORDER,
                     BAR_Y + BAR_BORDER,
                     new_fill_width,
                     inner_bar_height,
                     COLOR_BAR_FILL);
    }
}
