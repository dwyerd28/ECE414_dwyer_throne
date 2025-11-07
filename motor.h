// motor.h
#pragma once
#include <stdint.h>

// Track the current position of the motors
.

typedef struct {
    int32_t x_steps;
    int32_t y_steps;
} MotorPose;


// Initializes GPIO pins for stepper drivers


void motor_init(void);
void motor_pen_init(void);


// Moves the pen servo up or down.

void motor_pen_up(void);
void motor_pen_down(void);


// Moves both X and Y steppers to given
// (x_mm, y_mm) coordinates in millimeters.



void motor_move_to_mm(float x_mm, float y_mm);


MotorPose motor_get_pose(void);
