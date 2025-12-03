cmake_minimum_required(VERSION 3.13)

 include(pico_sdk_import.cmake)


project(ECE414_Final)

pico_sdk_init()

# ---- SOURCE FILES ----
add_executable(ECE414_Final)

pico_generate_pio_header(<your_target> ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)

target_sources(ECE414_Final PRIVATE
    main.c
    motor.c     # GPIO-based control for two NEMA17 motors
    limit.c      # Establishes the limits for X and Y positions
    coord_setup.c
    glcdfont.c
    lcd_progress.c
    TFTMaster.c
    TouchScreen.c
    ts_lcd.c


)

# ---- LIBRARIES ----
target_link_libraries(ECE414_Final PRIVATE
    pico_stdlib     
    hardware_pwm    
    hardware_gpio    
    hardware_i2c     
    hardware.pio
    hardware.adc
)

pico_enable_stdio_usb(<your_target> 1)   
pico_enable_stdio_uart(<your_target> 1)


# ---- BUILD OUTPUT ----
pico_add_extra_outputs(ECE414_Final)
