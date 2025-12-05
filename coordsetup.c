// coordsetup.c
#include "coordsetup.h"



static const DrawPoint g_path[] = {
    // Travel move from (0,0) to (20,20)
    {  0.0f,  0.0f, false },  // start (assumed current pos)
    { 20.0f, 20.0f, false },  // move with pen up

    // Now draw a square 40x40 mm
    { 20.0f, 20.0f, true  },  // pen down, start of square
    { 60.0f, 20.0f, true  },
    { 60.0f, 60.0f, true  },
    { 20.0f, 60.0f, true  },
    { 20.0f, 20.0f, true  },  // back to start

    // Lift pen and move back home
    {  0.0f,  0.0f, false },
    


   
};

size_t coord_get_count(void) {
    return sizeof(g_path)/sizeof(g_path[0]);
}

const DrawPoint *coord_get_point(size_t index) {
    if (index >= coord_get_count()) return NULL;
    return &g_path[index];
}

