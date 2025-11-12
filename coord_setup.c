#include "coord_setup.h"
#include "config.h"



//   - draw a 20×20 square
//   - travel (pen up) back home (0,0)

static const Point kDemo[] = {
    {  0,  0, false},  
    { 10, 10, false},   
    { 30, 10, true },  
    { 30, 30, true },  
    { 10, 30, true },   
    { 10, 10, true },   
    {  0,  0, false}    
};

const Point* coord_get_path(size_t* out_len) {
    if (out_len != 0) {
        *out_len = sizeof(kDemo) / sizeof(kDemo[0]);
    }
    return kDemo;
}

float coord_steps_per_mm_x(void) { return STEPS_PER_MM_X; }
float coord_steps_per_mm_y(void) { return STEPS_PER_MM_Y; }

