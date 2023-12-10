set(PROJECT html)

file(GLOB FILES *.c *.cpp *.h *.hpp)

project(${PROJECT} C CXX ASM)
add_library(${PROJECT} STATIC ${FILES})
set_target_properties(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)
