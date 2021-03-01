add_executable(hello_serial)

target_sources(hello_serial PRIVATE hello_serial.c)

add_definitions(-DPICO_DEFAULT_UART_TX_PIN=16 -DPICO_DEFAULT_UART_RX_PIN=17)

# Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(hello_serial pico_stdlib)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(hello_serial)
