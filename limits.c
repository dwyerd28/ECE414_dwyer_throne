
#include "limits.h"

//allowed build size
#define X_MIN_MM   0.0f
#define X_MAX_MM 200.0f   // change to your real X travel
#define Y_MIN_MM   0.0f
#define Y_MAX_MM 200.0f   // change to your real Y travel

void limits_init(void) {

    
}

bool limits_within(float x_mm, float y_mm) {
    if (x_mm < X_MIN_MM || x_mm > X_MAX_MM) return false;
    if (y_mm < Y_MIN_MM || y_mm > Y_MAX_MM) return false;
    return true;
}



