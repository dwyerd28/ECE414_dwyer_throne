
#pragma once
#include <stddef.h>

// Initialize TFT LCD and draw static elements
void ui_lcd_init(void);

// Update percent text & progress bar based on completed points
void ui_lcd_update_progress(size_t done, size_t total);

// Show a "Done" message when drawing complete
void ui_lcd_show_done(void);
