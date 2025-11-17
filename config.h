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
#define COORD_DIFF_THRESH_MM 10.0f

// changes to 1 to flip motor direction
#define INVERT_MOTOR_A_DIR 0
#define INVERT_MOTOR_B_DIR 0

