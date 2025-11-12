#pragma once
#include <stdint.h>

typedef struct {
    int32_t x_steps;
    int32_t y_steps;
} MotorPose;

void motor_init(void);      // setup STEP/DIR pins
void motor_pen_init(void);  // setup PWM for servo and raise pen
void motor_pen_up(void);    // pen lift 
void motor_pen_down(void);  // pen down 


void motor_move_to_mm(float x_mm, float y_mm); 
MotorPose motor_get_pose(void);   
            
