// motor.c
#include "motor.h"
#include "pins.h"
#include "pico/stdlib.h"
#include <stdint.h>

// Steps per millimeter in FULL STEP mode with GT2 (2 mm pitch) and 20T pulley:
// 200 steps/rev / 40 mm/rev = 5 steps/mm
#define STEPS_PER_MM          5.0f

// Step timing parameters (tune for speed)
// Pulse width to satisfy DRV8825 timing
#define STEPPER_PULSE_US      3      // high time on STEP pin
#define STEPPER_STEP_DELAY_US 1000   // delay between micro-steps (speed)

// Current Cartesian position in *steps*
static int32_t cur_x_steps = 0;
static int32_t cur_y_steps = 0;

// Current motor A/B positions in steps (CoreXY)
static int32_t cur_a_steps = 0;
static int32_t cur_b_steps = 0;

static inline int32_t mm_to_steps(float mm) {
    float s = mm * STEPS_PER_MM;
    if (s >= 0.0f) s += 0.5f; else s -= 0.5f; // round to nearest
    return (int32_t)s;
}

// Low-level function: step both motors according to CoreXY deltas
static void motor_step_corexy(int32_t da, int32_t db) {
    int32_t steps_a = (da >= 0) ? da : -da;
     int32_t steps_b = (db >= 0) ? db : -db;
   

    // Set direction pins
    // gpio_put(PIN_MOTOR_A_DIR, (da >= 0) ? 1 : 0);
    // gpio_put(PIN_MOTOR_B_DIR, (db >= 0) ? 1 : 0);
       gpio_put(PIN_MOTOR_A_DIR, (da >= 0));
    gpio_put(PIN_MOTOR_B_DIR, (db >= 0));

    int32_t max_steps = (steps_a > steps_b) ? steps_a : steps_b;
    if (max_steps == 0) return;

    int32_t err_a = max_steps / 2;
    int32_t err_b = max_steps / 2;
    int32_t done_a = 0;
    int32_t done_b = 0;

    for (int32_t i = 0; i < max_steps; i++) {
        bool stepA = false;
        bool stepB = false;

        err_a -= steps_a;
        if (err_a < 0 && done_a < steps_a) {
            err_a += max_steps;
            stepA = true;
            done_a++;
            cur_a_steps += (da >= 0) ? 1 : -1;
        }

        err_b -= steps_b;
        if (err_b < 0 && done_b < steps_b) {
            err_b += max_steps;
            stepB = true;
            done_b++;
            cur_b_steps += (db >= 0) ? 1 : -1;
        }

        // Generate STEP pulses
        if (stepA) gpio_put(PIN_MOTOR_A_STEP, 1);
        if (stepB) gpio_put(PIN_MOTOR_B_STEP, 1);

        sleep_us(STEPPER_PULSE_US);

        if (stepA) gpio_put(PIN_MOTOR_A_STEP, 0);
        if (stepB) gpio_put(PIN_MOTOR_B_STEP, 0);

        sleep_us(STEPPER_STEP_DELAY_US);
    }
}

void motor_init(void) {
    gpio_init(PIN_MOTOR_A_STEP);
    gpio_set_dir(PIN_MOTOR_A_STEP, GPIO_OUT);
    gpio_put(PIN_MOTOR_A_STEP, 0);

    gpio_init(PIN_MOTOR_A_DIR);
    gpio_set_dir(PIN_MOTOR_A_DIR, GPIO_OUT);
    gpio_put(PIN_MOTOR_A_DIR, 0);

    gpio_init(PIN_MOTOR_B_STEP);
    gpio_set_dir(PIN_MOTOR_B_STEP, GPIO_OUT);
    gpio_put(PIN_MOTOR_B_STEP, 0);

    gpio_init(PIN_MOTOR_B_DIR);
    gpio_set_dir(PIN_MOTOR_B_DIR, GPIO_OUT);
    gpio_put(PIN_MOTOR_B_DIR, 0);

    cur_x_steps = cur_y_steps = 0;
    cur_a_steps = cur_b_steps = 0;
}

// Move in mm using CoreXY kinematics
bool motor_move_to_mm(float x_mm, float y_mm) {
    int32_t target_x_steps = mm_to_steps(x_mm);
    int32_t target_y_steps = mm_to_steps(y_mm);

    int32_t dx = target_x_steps - cur_x_steps;
    int32_t dy = target_y_steps - cur_y_steps;

    // CoreXY transform: A = X + Y, B = X - Y
    int32_t da = (dx + dy);
    int32_t db = (dx - dy);

    

    motor_step_corexy(da, db);

    cur_x_steps = target_x_steps;
    cur_y_steps = target_y_steps;
    return true;
}

void motor_emergency_stop(void) {
    // For now, just stop stepping. If you wire ENABLE pins,
    // you can also disable the drivers here.
    gpio_put(PIN_MOTOR_A_STEP, 0);
    gpio_put(PIN_MOTOR_B_STEP, 0);
}

float motor_get_x_mm(void) {
    return (float)cur_x_steps / STEPS_PER_MM;
}

float motor_get_y_mm(void) {
    return (float)cur_y_steps / STEPS_PER_MM;
}

