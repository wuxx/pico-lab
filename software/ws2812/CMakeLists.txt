add_executable(pico_ws2812)

pico_generate_pio_header(pico_ws2812 ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

target_sources(pico_ws2812 PRIVATE ws2812.c)

target_link_libraries(pico_ws2812 PRIVATE pico_stdlib pico_multicore hardware_pio hardware_interp)
pico_add_extra_outputs(pico_ws2812)

