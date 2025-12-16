#pragma once
#include <stdbool.h>

// Initialize limits 
void limits_init(void);

// returns true if (x_mm, y_mm) is inside allowed area
bool limits_within(float x_mm, float y_mm);


