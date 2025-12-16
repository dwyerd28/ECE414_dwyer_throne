#pragma once
#include <stddef.h>
#include <stdbool.h>

//structure for coordsetup.c
typedef struct {
    float x_mm;    
    float y_mm;      
    bool  pen_down;  
} DrawPoint;

// number of points in a path
size_t coord_get_count(void);


const DrawPoint *coord_get_point(size_t index);

