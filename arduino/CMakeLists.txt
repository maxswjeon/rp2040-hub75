file(GLOB FILES *.c *.cpp *.h *.hpp *.pio)

add_executable(${PROJECT} ${FILES})

pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)
pico_add_extra_outputs(${PROJECT})

target_include_directories(${PROJECT} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${UBXLIB_INC}
)

target_include_directories(${PROJECT} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${UBXLIB_PRIVATE_INC}
)

add_subdirectory(hub75)
add_subdirectory(html)
# add_subdirectory(wifinina) 

target_link_libraries(${PROJECT}
    pico_runtime
    pico_stdlib
    pico_multicore
    pico_rand
    pico_malloc
    hardware_i2c
    hardware_spi
    hardware_dma
    hardware_clocks
    hardware_pio
    hardware_flash
    html
    # wifinina
    hub75
)
