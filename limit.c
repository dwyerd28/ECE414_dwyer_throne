#include "limits.h"

// Module-private bounds
static float XN = 0.0f, XX = 0.0f;
static float YN = 0.0f, YX = 0.0f;

void limits_init(float xmin, float xmax, float ymin, float ymax) {
    XN = xmin;  XX = xmax;
    YN = ymin;  YX = ymax;
}

bool limits_point_ok_mm(float x_mm, float y_mm) {
    if (x_mm < XN) return false;
    if (x_mm > XX) return false;
    if (y_mm < YN) return false;
    if (y_mm > YX) return false;
    return true;
}

void limits_get_bounds(float* xmin, float* xmax, float* ymin, float* ymax) {
    if (xmin != 0) *xmin = XN;
    if (xmax != 0) *xmax = XX;
    if (ymin != 0) *ymin = YN;
    if (ymax != 0) *ymax = YX;
}
