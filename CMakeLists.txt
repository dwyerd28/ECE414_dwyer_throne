cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ECE414_Final)

pico_sdk_init()

# ---- SOURCE FILES ----
add_executable(ECE414_Final
    main.c
    motor.c     # GPIO-based control for two NEMA17 motors
   # servo.c       # SG90 servo driver
    limits.c      # Establishes the limits for X and Y positions


)

# ---- LIBRARIES ----
target_link_libraries(ECE414_Final
    pico_stdlib     # basic Pico features
    hardware_pwm     # SG90 servo
    hardware_gpio    # GPIO steppers
    hardware_i2c     # LCD (I2C)
)

# ---- ENABLE USB STDIO (optional) ----
pico_enable_stdio_usb(ECE414_Final 1)
pico_enable_stdio_uart(ECE414_Final 0)

# ---- BUILD OUTPUT ----
pico_add_extra_outputs(ECE414_Final)
