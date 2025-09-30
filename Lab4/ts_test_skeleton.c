// File: ts_test.c - Code Skeleton
// Prepared by Prof. Priyank Kalgaonkar


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "stdbool.h"
#include "inttypes.h"
#include "TouchScreen.h"
#include "TFTMaster.h"
#include "ts_lcd.h"

// TODO: Define buffers for storing coordinate strings
static char buffer1[30], buffer2[30];
// TODO: Define state tracking variables
static bool screenTouched, firstscreenTouched;
static uint16_t last_lcdx, last_lcdy; // Store last touch coordinates

void writeRAWCoor(struct TSPoint p) {
    // TODO: Set cursor position for raw coordinates display
    // HINT: Use tft_setCursor(x, y) - choose visible screen location
    tft_setCursor(500, 500);
    // TODO: Set text color for raw coordinates
    // HINT: Use tft_setTextColor(color) - pick a distinctive color
    tft_setTextColor(0x07FF);
    // TODO: Format raw coordinates into buffer1
    // HINT: Use sprintf(buffer1, "x: %d, y: %d, z: %d", p.x, p.y, p.z);
    sprintf(buffer1, "x: %d, y: %d, z: %d", p.x, p.y, p.z);
    // TODO: Display the formatted string
    // HINT: Use tft_writeString(buffer1);
    tft_writeString(buffer1);
}

void writeLCDCoor(uint16_t xcoor, uint16_t ycoor) {
    // TODO: Set cursor position for LCD coordinates display
    // HINT: Choose a different location than raw coordinates
    tft_setCursor(1000, 1000);
    // TODO: Set text color for LCD coordinates  
    // HINT: Use a different color than raw coordinates for clarity
    tft_setTextColor(0xFFE0);
    // TODO: Format LCD coordinates into buffer2
    // HINT: sprintf(buffer2, "x: %d, y: %d", xcoor, ycoor);
    sprintf(buffer2, "x: %d, y: %d", xcoor, ycoor);
    // TODO: Display the formatted string
    tft_writeString(buffer2);
}

void drawCrossHair(uint16_t xcoor, uint16_t ycoor) {
    // TODO: Draw a circle at the touch point (10 pixel diameter)
    // HINT: tft_drawCircle(xcoor, ycoor, radius, color) - radius = 5 for 10px diameter
    tft_drawCircle(xcoor, ycoor, 5, 0x07FF);
    // TODO: Draw horizontal line through center (10 pixels long)
    // HINT: tft_drawLine(xcoor-5, ycoor, xcoor+5, ycoor, color)
    tft_drawLine(xcoor-5, ycoor, xcoor+5, ycoor, 0x07FF);
    // TODO: Draw vertical line through center (10 pixels long)  
    // HINT: tft_drawLine(xcoor, ycoor-5, xcoor, ycoor+5, color)
    tft_drawLine(xcoor, ycoor-5, xcoor, ycoor+5, 0x07FF);
    // TODO: Draw a center pixel for better visibility
    // HINT: tft_drawPixel(xcoor, ycoor, color)
    tft_drawPixel(xcoor, ycoor, 0x07FF);
}

void deleteCrossHair(uint16_t xcoor, uint16_t ycoor) {
    // TODO: Erase the crosshair by drawing over it with background color
    // HINT: Use the same drawing functions as drawCrossHair but with ILI9340_BLACK
    // This creates the "erase" effect by overwriting with background color
    tft_drawCircle(xcoor, ycoor, 5, ILI9340_BLACK);
    tft_drawLine(xcoor-5, ycoor, xcoor+5, ycoor, ILI9340_BLACK);
    tft_drawLine(xcoor, ycoor-5, xcoor, ycoor+5, ILI9340_BLACK);
    tft_drawPixel(xcoor, ycoor, ILI9340_BLACK)

}

void clearScreen(char bufferx[30], char buffery[30]) {
    // TODO: Clear the raw coordinates text by overwriting with background color
    // HINT: Set cursor to raw coordinates position, set text color to BLACK, write the old string
    tft_setCursor(500,500);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString(buffer1);
    // TODO: Clear the LCD coordinates text similarly
    tft_setCursor(1000,1000);
    tft_setTextColor(ILI9340_BLACK);
    tft_writeString(buffer1);
    // TODO: Erase the previous crosshair
    // HINT: Call deleteCrossHair with the last known coordinates
    deleteCrossHair(last_lcdx, last_lcdy);
}

int main() {
    // TODO: Initialize the touchscreen and LCD system
    // HINT: Call the initialization function from ts_lcd.h
    ts_lcd_init();
    // TODO: Initialize the first touch flag to false
    firstscreenTouched = false;
    while (1) {
        // TODO: Clear previous frame's display elements
        // HINT: Call your clearScreen function
        clearScreen();
        // TODO: Get raw touchscreen coordinates
        // HINT: Create a TSPoint struct and call getPoint(&p)
        struct TSPoint tpoint;
        getPoint(&p);
        // TODO: Display raw coordinates on screen
        // HINT: Call writeRAWCoor with the point data
        writeRAWCoor(tpoint);
        
        // TODO: Convert to LCD coordinates and check if screen is touched
        uint16_t lcdx, lcdy;
        // HINT: Call get_ts_lcd(&lcdy, &lcdx) - note the parameter order!
        // HINT: Store the return value to know if screen is touched
        screenTouched = get_ts_lcd(&lcdy, &lcdx);
        
        // TODO: Update last coordinates if screen is currently touched
        if (screenTouched) {
            // HINT: Set first touch flag to true
            firstscreenTouched = true;
            // HINT: Update last_lcdx and last_lcdy with current coordinates
        }
        
        // TODO: Display last coordinates and crosshair if screen has ever been touched
        if (firstscreenTouched) {
            // HINT: Display the LCD coordinates
            tft_writeString(buffer2);
            // HINT: Draw the crosshair at the last touch location
            // NOTE: You may need to adjust coordinates based on screen orientation
            drawCrossHair(last_lcdx, last_lcdy);
        }
        
        // TODO: Add small delay to prevent flickering
        sleep_ms(100);
    }
}