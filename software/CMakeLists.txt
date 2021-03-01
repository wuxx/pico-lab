cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

project(pico_demos C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_compile_options(-Wall)

add_definitions(-DPICO_DEFAULT_UART_TX_PIN=16 -DPICO_DEFAULT_UART_RX_PIN=17)

include_directories(include)

add_subdirectory(infones)
add_subdirectory(ws2812)
add_subdirectory(hello_serial)
