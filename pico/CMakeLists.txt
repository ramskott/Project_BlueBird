cmake_minimum_required(VERSION 3.13)

# Include Raspberry Pi Pico SDK
include(pico_sdk_import.cmake OPTIONAL)

project(bluebird_sparrow_classifier C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Check Pico SDK
if (NOT PICO_SDK_PATH)
    message(WARNING "PICO_SDK_PATH environment variable is not set.")
endif()

pico_sdk_init()

# Executable target
add_executable(bluebird_pico
    main.cc
    model_data.cc
)

# Link Pico libraries and pico-tflmicro
target_link_libraries(bluebird_pico
    pico_stdlib
    hardware_adc
    hardware_dma
    hardware_timer
    pico_tflmicro
)

# Enable USB serial output and disable UART (or enable UART as desired)
pico_enable_stdio_usb(bluebird_pico 1)
pico_enable_stdio_uart(bluebird_pico 0)

# Create .uf2, .bin, .hex for flashing
pico_add_extra_outputs(bluebird_pico)
