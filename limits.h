#pragma once
#include <stdbool.h>

void limits_init(float xmin, float xmax, float ymin, float ymax);
bool limits_point_ok_mm(float x_mm, float y_mm);
void limits_get_bounds(float* xmin, float* xmax, float* ymin, float* ymax);
