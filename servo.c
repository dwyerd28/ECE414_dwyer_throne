
#include "servo.h"
#include "pins.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"


#define SERVO_PERIOD_US 20000

//movement of servo
//tuned these until it worked on the robot
#define SERVO_PEN_UP_US   1000   // ~1.0 ms pulse
#define SERVO_PEN_DOWN_US 2000   // ~2.0 ms pulse

static uint slice_num;

static void servo_set_pulse_us(uint16_t us) {
    if (us > SERVO_PERIOD_US) us = SERVO_PERIOD_US;
    pwm_set_gpio_level(PIN_SERVO, us);
}

void servo_init(void) {
    gpio_set_function(PIN_SERVO, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(PIN_SERVO);

   
    pwm_set_wrap(slice_num, SERVO_PERIOD_US);
    pwm_set_clkdiv(slice_num, 125.0f); 

    pwm_set_enabled(slice_num, true);

    // Default to pen up
    servo_set_pulse_us(SERVO_PEN_UP_US);
}

void pen_up(void) {
    servo_set_pulse_us(SERVO_PEN_UP_US);
    //gives servo time to move
    sleep_ms(200); 
}

void pen_down(void) {
    servo_set_pulse_us(SERVO_PEN_DOWN_US);
    sleep_ms(200); 
}


