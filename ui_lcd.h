
#pragma once
#include <stddef.h>

// initialize TFT LCD and draw static elements
void ui_lcd_init(void);

// update percent text & progress bar based on completed points
void ui_lcd_update_progress(size_t done, size_t total);

//shows done once completed
void ui_lcd_show_done(void);

