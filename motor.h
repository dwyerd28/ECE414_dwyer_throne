// motor.h
#pragma once
#include <stdbool.h>

// Initialize stepper GPIO and internal position tracking
void motor_init(void);

// Move in Cartesian coordinates (mm) using CoreXY kinematics.
// Returns true on success.
bool motor_move_to_mm(float x_mm, float y_mm);

// Immediately stop (used if limits exceeded or fault)
void motor_emergency_stop(void);

// Optional helpers
float motor_get_x_mm(void);
float motor_get_y_mm(void);

