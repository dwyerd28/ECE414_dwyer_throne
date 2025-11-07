// coord_setup.h
#pragma once
#include <stddef.h>
#include <stdbool.h>


// Structure that defines one coordinate point
// x, y: target position in millimeters


typedef struct {
    int x;
    int y;
    bool draw;
} Point;


// Returns pointer to static array of path points.


const Point* coord_get_path(size_t* out_len);

// Returns how many stepper steps per millimeter for X and Y axes.
float coord_steps_per_mm_x(void);
float coord_steps_per_mm_y(void);


