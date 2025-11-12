#pragma once
#include <stddef.h>
#include <stdbool.h>

// One path point in *millimeters*
typedef struct {
    int x;      
    int y;       
    bool draw;   

} Point;

// Returns pointer to static path and its length
const Point* coord_get_path(size_t* out_len);

// Expose current steps-per-mm calibration for each axis
float coord_steps_per_mm_x(void);
float coord_steps_per_mm_y(void);



