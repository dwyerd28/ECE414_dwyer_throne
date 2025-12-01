
#ifndef LCD_PROGRESS_H
#define LCD_PROGRESS_H

#include <stddef.h>
#include <stdint.h>

// Initialize the LCD and draw the static UI:
void lcd_progress_init(size_t total_points);

// Update the LCD progress:
void lcd_progress_update(size_t current_index, size_t total_points);

#endif // LCD_PROGRESS_H
