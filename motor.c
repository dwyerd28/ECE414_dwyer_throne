#include "motor.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hw_pins.h"
#include "coord_setup.h"
#include "config.h"

// We keep pose in *cartesian* steps (X,Y), not motor A/B steps.
static MotorPose pose = {0, 0};

// Generate one step pulse
static inline void pulse(uint pin, uint32_t us_high, uint32_t us_low) {
    gpio_put(pin, 1);
    sleep_us(us_high);
    gpio_put(pin, 0);
    sleep_us(us_low);
}

// Step two *motors* (A and B) by integer deltas (da, db).
static void step_two_motors(int32_t da, int32_t db) {
    int sa; 
    int sb; 

    if (da >= 0) { 
        sa = 1; 
    } else { 
        sa = -1; 
    }
    if (db >= 0) { 
        sb = 1; 
    } else { 
        sb = -1; 
    }

    // apply optional direction inversions from config
    if (INVERT_MOTOR_A_DIR) { sa = -sa; }
    if (INVERT_MOTOR_B_DIR) { sb = -sb; }

    // magnitudes
    da = sa * da;
    db = sb * db;

    // set DIR pins
    if (sa > 0) { 
        gpio_put(PIN_MOTOR_A_DIR, 1); 
    } else { 
        gpio_put(PIN_MOTOR_A_DIR, 0); 
    }
    if (sb > 0) { 
        gpio_put(PIN_MOTOR_B_DIR, 1); 
    } else { 
        gpio_put(PIN_MOTOR_B_DIR, 0); 
    }


    int32_t err;
    if (da > db) { err = da; } else { err = -db; }
    err = err / 2;

    int32_t a = 0;
    int32_t b = 0;

    while ((a < da) || (b < db)) {
        int32_t e2 = err;

        if ((e2 > -da) && (a < da)) {
            err = err - db;
            a = a + 1;
            pulse(PIN_MOTOR_A_STEP, 2, 2);
        }
        if ((e2 <  db) && (b < db)) {
            err = err + da;
            b = b + 1;
            pulse(PIN_MOTOR_B_STEP, 2, 2);
        }
    }
}

// Servo helpers 
static void servo_pwm_setup(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 64.f);
    pwm_init(slice, &cfg, true);
}

static void servo_write_us(uint gpio, uint us) {
    uint slice = pwm_gpio_to_slice_num(gpio);
    uint16_t wrap = 20000;  
    pwm_set_wrap(slice, wrap);
    pwm_set_gpio_level(gpio, (uint16_t)((us * wrap) / 20000));
}

// Public API 
void motor_init(void) {
    gpio_init(PIN_MOTOR_A_STEP); gpio_set_dir(PIN_MOTOR_A_STEP, GPIO_OUT);
    gpio_init(PIN_MOTOR_A_DIR ); gpio_set_dir(PIN_MOTOR_A_DIR , GPIO_OUT);
    gpio_init(PIN_MOTOR_B_STEP); gpio_set_dir(PIN_MOTOR_B_STEP, GPIO_OUT);
    gpio_init(PIN_MOTOR_B_DIR ); gpio_set_dir(PIN_MOTOR_B_DIR , GPIO_OUT);
}

void motor_pen_init(void) { servo_pwm_setup(PIN_SERVO); motor_pen_up(); }
void motor_pen_up(void)   { servo_write_us(PIN_SERVO, 1100); }
void motor_pen_down(void) { servo_write_us(PIN_SERVO, 1700); }

void motor_move_to_mm(float x_mm, float y_mm) {
    // Convert target CARTESIAN mm → absolute CARTESIAN steps
    int32_t tx = (int32_t)(x_mm * coord_steps_per_mm_x() + 0.5f);
    int32_t ty = (int32_t)(y_mm * coord_steps_per_mm_y() + 0.5f);

    // Cartesian deltas
    int32_t dx = tx - pose.x_steps;
    int32_t dy = ty - pose.y_steps;

    // CoreXY mapping
    // Motor A tracks (dx + dy); Motor B tracks (dx - dy).
    int32_t da = dx + dy;
    int32_t db = dx - dy;

    // Step motors by (da, db)
    step_two_motors(da, db);

    // Update 
    pose.x_steps = pose.x_steps + dx;
    pose.y_steps = pose.y_steps + dy;
}

MotorPose motor_get_pose(void) { 
    return pose;
 }
