#pragma once

//Scaling
#define STEPS_PER_MM_X 80.0f
#define STEPS_PER_MM_Y 80.0f

// workspace limits
#define X_MIN_MM 0.0f
#define X_MAX_MM 120.0f
#define Y_MIN_MM 0.0f
#define Y_MAX_MM 120.0f

// timing
#define PEN_SETTLE_MS 150  //delay
#define FSM_TICK_MS   5     


//if distance (in mm) from current coordinate to next coordinate is 
//greater than this we go to TRAVEL (pen up)
#define COORD_DIFF_THRESH_MM 10.0f

