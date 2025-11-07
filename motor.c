// motor.c
#include "motor.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hw_pins.h"
#include "coord_setup.h"

// Store the current motor position
static MotorPose pose = {0, 0};


// Generate one step pulse on a given pin.
// us_high: microseconds high time
// us_low : microseconds low time

static inline void pulse(uint pin, uint32_t us_high, uint32_t us_low) {
    gpio_put(pin, 1);
    sleep_us(us_high);
    gpio_put(pin, 0);
    sleep_us(us_low);
}

// Move both axes together along a straight line.

// dx, dy = difference in steps to move.

static void step_line(int32_t dx, int32_t dy) {
    int sx;
    int sy;

    // Determine direction signs
    if (dx >= 0) { 
        sx = 1; 
    } else { 
        sx = -1; 
    }
    if (dy >= 0) { 
        sy = 1; 
    }  else { 
        sy = -1; 
    }

    // Use absolute magnitudes
    dx = sx * dx;
    dy = sy * dy;

    // Set motor direction pins
    if (sx > 0) { 
        gpio_put(PIN_X_DIR, 1); 
    } else { 
        gpio_put(PIN_X_DIR, 0); 
    }

    if (sy > 0) { 
        gpio_put(PIN_Y_DIR, 1); 
    } else { 
        gpio_put(PIN_Y_DIR, 0); 
    }

    
    int32_t err;
    if (dx > dy) { err = dx; }
    else { err = -dy; }
    err = err / 2;

    int32_t x = 0;
    int32_t y = 0;

    // Loop until both axes reach their targets
    while ((x < dx) || (y < dy)) {
        int32_t e2 = err;

        // Step X if needed
        if ((e2 > -dx) && (x < dx)) {
            err = err - dy;
            x = x + 1;
            pulse(PIN_X_STEP, 2, 2);  
        }

        // Step Y if needed
        if ((e2 < dy) && (y < dy)) {
            err = err + dx;
            y = y + 1;
            pulse(PIN_Y_STEP, 2, 2);
        }
    }

    // Update absolute pose after motion
    pose.x_steps = pose.x_steps + (sx * x);
    pose.y_steps = pose.y_steps + (sy * y);
}


// Initialize servo PWM hardware on its GPIO pin.

static void servo_pwm_setup(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 64.f);  
    pwm_init(slice, &cfg, true);
}


// Send a servo pulse width in microseconds


static void servo_write_us(uint gpio, uint us) {
    uint slice = pwm_gpio_to_slice_num(gpio);
    uint16_t top = 20000;                   
    pwm_set_wrap(slice, top);
    uint16_t level = (uint16_t)((us * top) / 20000);
    pwm_set_gpio_level(gpio, level);
}


// Initialize motor GPIO directions

void motor_init(void) {
    gpio_init(PIN_X_STEP); gpio_set_dir(PIN_X_STEP, GPIO_OUT);
    gpio_init(PIN_X_DIR ); gpio_set_dir(PIN_X_DIR , GPIO_OUT);
    gpio_init(PIN_Y_STEP); gpio_set_dir(PIN_Y_STEP, GPIO_OUT);
    gpio_init(PIN_Y_DIR ); gpio_set_dir(PIN_Y_DIR , GPIO_OUT);
}


// Initialize servo and raise pen initially

void motor_pen_init(void) {
    servo_pwm_setup(PIN_SERVO);
    motor_pen_up();
}


// Raise or lower the pen.

void motor_pen_up(void)   { servo_write_us(PIN_SERVO, 1100); }  // pen lifted
void motor_pen_down(void) { servo_write_us(PIN_SERVO, 1700); }  // pen touches paper


// Convert mm targets to steps and call line mover.
// Blocking call — waits until motion complete.

void motor_move_to_mm(float x_mm, float y_mm) {
    int32_t tx = (int32_t)(x_mm * coord_steps_per_mm_x() + 0.5f);
    int32_t ty = (int32_t)(y_mm * coord_steps_per_mm_y() + 0.5f);
    int32_t dx = tx - pose.x_steps;
    int32_t dy = ty - pose.y_steps;
    step_line(dx, dy);  
}


// Return current absolute step position.

MotorPose motor_get_pose(void) { return pose; }
