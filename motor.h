#pragma once
#include <stdbool.h>

//initialize stepper GPIO
void motor_init(void);


//move in mm using CoreXY kinematics
bool motor_move_to_mm(float x_mm, float y_mm);

// immeditely stop
void motor_emergency_stop(void);

// Optional helpers
float motor_get_x_mm(void);
float motor_get_y_mm(void);


