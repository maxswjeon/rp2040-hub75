file(GLOB FILES *.c *.cpp *.h *.hpp)

pico_generate_pio_header(${PROJECT} ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)

target_include_directories(${PROJECT} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)