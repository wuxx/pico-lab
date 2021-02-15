add_executable(pico_infones)

pico_generate_pio_header(pico_infones ${CMAKE_CURRENT_LIST_DIR}/ili9341_lcd.pio)

target_sources(pico_infones PRIVATE main.c
                infones/InfoNES.c  
                infones/InfoNES_Mapper.c  
                infones/InfoNES_pAPU.c  
                infones/InfoNES_System.c  
                infones/K6502.c  
                 )

target_include_directories(pico_infones PRIVATE infones/)


target_link_libraries(pico_infones PRIVATE pico_stdlib pico_multicore hardware_pio hardware_interp)
pico_add_extra_outputs(pico_infones)

