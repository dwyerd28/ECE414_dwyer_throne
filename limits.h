// limits.h
#pragma once
#include <stdbool.h>

// Initialize limits (if needed)
void limits_init(void);

// Returns true if (x_mm, y_mm) is inside allowed area
bool limits_within(float x_mm, float y_mm);

