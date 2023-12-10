set(PROJECT hub75)

file(GLOB FILES *.c *.cpp *.h *.hpp *.pio)

project(${PROJECT} C CXX ASM)
add_library(${PROJECT} STATIC ${FILES})
set_target_properties(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT} 
    pico_runtime
    pico_stdlib
    pico_malloc
    hardware_pio
)
target_include_directories(${PROJECT} PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/..)

pico_generate_pio_header(${PROJECT} ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)
