#include "coordsetup.h"



static const DrawPoint g_path[] = {
    
    {  0.0f,  0.0f, false },  
    { 20.0f, 20.0f, false },  
//square
    { 20.0f, 20.0f, true  },  
    { 60.0f, 20.0f, true  },
    { 60.0f, 60.0f, true  },
    { 20.0f, 60.0f, true  },
    { 20.0f, 20.0f, true  },  


    {  0.0f,  0.0f, false },
    


   
};

size_t coord_get_count(void) {
    return sizeof(g_path)/sizeof(g_path[0]);
}

const DrawPoint *coord_get_point(size_t index) {
    if (index >= coord_get_count()) return NULL;
    return &g_path[index];
}


