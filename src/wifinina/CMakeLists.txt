set(PROJECT wifinina)

file(GLOB FILES *.c *.cpp *.h *.hpp *.pio)

project(${PROJECT} C CXX ASM)
add_library(${PROJECT} STATIC ${FILES})
set_target_properties(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT} 
    pico_runtime
    pico_stdlib
    pico_malloc
    hardware_spi
)
target_include_directories(${PROJECT} PUBLIC .)

add_subdirectory(utility)
add_subdirectory(arduino)