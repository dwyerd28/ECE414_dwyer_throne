#include "motor.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hw_pins.h"
#include "coord_setup.h"

// Current position (in steps) tracked by this module
static MotorPose pose = {0, 0};


static inline void pulse(uint pin, uint32_t us_high, uint32_t us_low) {
    gpio_put(pin, 1);
    sleep_us(us_high);
    gpio_put(pin, 0);
    sleep_us(us_low);
}


static void step_line(int32_t dx, int32_t dy) {
    int sx;
    int sy;

    // determine direction signs (+1 or -1) 
    if (dx >= 0) { sx = 1; } else { sx = -1; }
    if (dy >= 0) { sy = 1; } else { sy = -1; }
    dx = sx * dx;
    dy = sy * dy;

    // set DIR outputs to match desired directions
    if (sx > 0) { gpio_put(PIN_X_DIR, 1); } else { gpio_put(PIN_X_DIR, 0); }
    if (sy > 0) { gpio_put(PIN_Y_DIR, 1); } else { gpio_put(PIN_Y_DIR, 0); }

 
    int32_t err;
    if (dx > dy) { err = dx; } else { err = -dy; }
    err = err / 2;

    // step counters for this move (
    int32_t x = 0;
    int32_t y = 0;

    // loop until both axes reach their target steps
    while ((x < dx) || (y < dy)) {
        int32_t e2 = err;

        if ((e2 > -dx) && (x < dx)) {
            err = err - dy;
            x = x + 1;
            pulse(PIN_X_STEP, 2, 2);  
        }
        if ((e2 <  dy) && (y < dy)) {
            err = err + dx;
            y = y + 1;
            pulse(PIN_Y_STEP, 2, 2);
        }
    }

    // update absolute pose (steps) including direction signs
    pose.x_steps = pose.x_steps + (sx * x);
    pose.y_steps = pose.y_steps + (sy * y);
}

// ----- servo helpers -----
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


void motor_init(void) {
    gpio_init(PIN_X_STEP); gpio_set_dir(PIN_X_STEP, GPIO_OUT);
    gpio_init(PIN_X_DIR ); gpio_set_dir(PIN_X_DIR , GPIO_OUT);
    gpio_init(PIN_Y_STEP); gpio_set_dir(PIN_Y_STEP, GPIO_OUT);
    gpio_init(PIN_Y_DIR ); gpio_set_dir(PIN_Y_DIR , GPIO_OUT);
}

void motor_pen_init(void) {
    servo_pwm_setup(PIN_SERVO);
    motor_pen_up();  
}

void motor_pen_up(void)   { 
    servo_write_us(PIN_SERVO, 1100); 
} 
void motor_pen_down(void) { 
    servo_write_us(PIN_SERVO, 1700); 
} 

void motor_move_to_mm(float x_mm, float y_mm) {
    
    int32_t target_x = (int32_t)(x_mm * coord_steps_per_mm_x() + 0.5f);
    int32_t target_y = (int32_t)(y_mm * coord_steps_per_mm_y() + 0.5f);

    
    int32_t dx = target_x - pose.x_steps;
    int32_t dy = target_y - pose.y_steps;

    step_line(dx, dy);
}

MotorPose motor_get_pose(void) {
    return pose;
}
