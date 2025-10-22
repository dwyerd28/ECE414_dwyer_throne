cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(Lab3)

pico_sdk_init()

add_executable(Lab3
main.c
led_out.c
led_out.h
sw_in.c
sw_in.h
debounce_sw1.c
debounce_sw1.h
debounce_sw2.c
debounce_sw2.h
timer.c
timer.h
)

target_link_libraries(Lab3 pico_stdlib)

pico_add_extra_outputs(Lab3)
