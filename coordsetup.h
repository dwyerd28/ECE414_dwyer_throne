// coordsetup.h
#pragma once
#include <stddef.h>
#include <stdbool.h>

// A single point in the drawing path
typedef struct {
    float x_mm;    
    float y_mm;      
    bool  pen_down;  
} DrawPoint;

// Number of points in the path
size_t coord_get_count(void);

// Get pointer to point index (0..count-1)
const DrawPoint *coord_get_point(size_t index);
