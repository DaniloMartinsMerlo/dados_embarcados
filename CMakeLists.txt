cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(PICO_BOARD pico_w)
project(pico_telemetry C CXX ASM)
set(CMAKE_C_STANDARD 11)
pico_sdk_init()

add_executable(pico_telemetry
    main.c
    src/button_handler.c
    src/ldr_handler.c
    src/wifi_handler.c
)

target_include_directories(pico_telemetry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(pico_telemetry
    pico_stdlib
    pico_cyw43_arch_lwip_poll
    hardware_adc
    hardware_gpio
    hardware_timer
)

pico_enable_stdio_usb(pico_telemetry 1)
pico_enable_stdio_uart(pico_telemetry 0)
pico_add_extra_outputs(pico_telemetry)