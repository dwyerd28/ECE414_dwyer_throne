
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// CHANGE THESE TO MATCH YOUR WIRING
#define MOTOR_A_STEP 2
#define MOTOR_A_DIR  3
#define MOTOR_B_STEP 4
#define MOTOR_B_DIR  5

// Pulse timing — slow enough for testing
#define STEP_HIGH_US 500
#define STEP_LOW_US  500
#define TEST_STEPS   200

// Generate one step pulse
static inline void do_step(uint pin) {
    gpio_put(pin, 1);
    sleep_us(STEP_HIGH_US);
    gpio_put(pin, 0);
    sleep_us(STEP_LOW_US);
}

int main() {
    stdio_init_all();
    sleep_ms(500);

    // Initialize step and direction pins
    gpio_init(MOTOR_A_STEP); gpio_set_dir(MOTOR_A_STEP, GPIO_OUT);
    gpio_init(MOTOR_A_DIR);  gpio_set_dir(MOTOR_A_DIR,  GPIO_OUT);
    gpio_init(MOTOR_B_STEP); gpio_set_dir(MOTOR_B_STEP, GPIO_OUT);
    gpio_init(MOTOR_B_DIR);  gpio_set_dir(MOTOR_B_DIR,  GPIO_OUT);

    while (true) {

        // -------------------------------------------------------------
        // TEST 1: MOTOR A ONLY — Forward 200 steps
        // -------------------------------------------------------------
        gpio_put(MOTOR_A_DIR, 1);
        for (int i = 0; i < TEST_STEPS; i++) do_step(MOTOR_A_STEP);
        sleep_ms(1000);

        // MOTOR A Reverse
        gpio_put(MOTOR_A_DIR, 0);
        for (int i = 0; i < TEST_STEPS; i++) do_step(MOTOR_A_STEP);
        sleep_ms(1000);

        // -------------------------------------------------------------
        // TEST 2: MOTOR B ONLY — Forward 200 steps
        // -------------------------------------------------------------
        gpio_put(MOTOR_B_DIR, 1);
        for (int i = 0; i < TEST_STEPS; i++) do_step(MOTOR_B_STEP);
        sleep_ms(1000);

        // MOTOR B Reverse
        gpio_put(MOTOR_B_DIR, 0);
        for (int i = 0; i < TEST_STEPS; i++) do_step(MOTOR_B_STEP);
        sleep_ms(1000);

        // -------------------------------------------------------------
        // TEST 3: BOTH MOTORS SAME DIRECTION
        // (Should produce X movement in CoreXY)
        // -------------------------------------------------------------
        gpio_put(MOTOR_A_DIR, 1);
        gpio_put(MOTOR_B_DIR, 1);
        for (int i = 0; i < TEST_STEPS; i++) {
            do_step(MOTOR_A_STEP);
            do_step(MOTOR_B_STEP);
        }
        sleep_ms(1000);

        // -------------------------------------------------------------
        // TEST 4: MOTORS OPPOSITE DIRECTIONS
        // (Should produce Y movement in CoreXY)
        // -------------------------------------------------------------
        gpio_put(MOTOR_A_DIR, 1);
        gpio_put(MOTOR_B_DIR, 0);
        for (int i = 0; i < TEST_STEPS; i++) {
            do_step(MOTOR_A_STEP);
            do_step(MOTOR_B_STEP);
        }
        sleep_ms(2000);
    }
}


