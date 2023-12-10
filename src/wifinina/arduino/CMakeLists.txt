file(GLOB FILES *.c *.cpp *.h *.hpp *.pio)

target_include_directories(${PROJECT} PUBLIC .)
target_sources(${PROJECT} PUBLIC ${FILES})