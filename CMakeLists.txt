cmake_minimum_required(VERSION 3.13)

 include(pico_sdk_import.cmake)


project(ECE414_Final)

pico_sdk_init()

# ---- SOURCE FILES ----
add_executable(ECE414_Final)

pico_generate_pio_header(ECE414_Final ${CMAKE_CURRENT_LIST_DIR}/SPIPIO.pio)

target_sources(ECE414_Final PRIVATE
    main.c
    motor.c     # GPIO-based control for two NEMA17 motors
    limits.c      # Establishes the limits for X and Y positions
    coordsetup.c
    glcdfont.c
    ui_lcd.c
    servo.c
    TFTMaster.c
    


)

# ---- LIBRARIES ----
target_link_libraries(ECE414_Final PRIVATE
    pico_stdlib     
    hardware_pwm    
    hardware_gpio    
    hardware_i2c     
    hardware_pio
    hardware_adc
)

target_include_directories(ECE414_Final PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

pico_enable_stdio_usb(ECE414_Final 1)   
pico_enable_stdio_uart(ECE414_Final 1)


# ---- BUILD OUTPUT ----
pico_add_extra_outputs(ECE414_Final)


