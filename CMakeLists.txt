cmake_minimum_required(VERSION 3.25)

if (NOT PICO_SDK_PATH)
    if(DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
        message("Setting PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
    else()
        set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/pico-sdk)
        message("Setting PICO_SDK_PATH from git submodule path ('${PICO_SDK_PATH}')")
    endif()
else()
    message("PICO_SDK_PATH already set ('${PICO_SDK_PATH}')")
endif()

if (NOT ARDUINO_CORE_MBED_PATH)
    if (DEFINED ENV{ARDUINO_CORE_MBED_PATH})
        set(ARDUINO_CORE_MBED_PATH $ENV{ARDUINO_CORE_MBED_PATH})
        message("Setting ARDUINO_CORE_MBED_PATH from environment ('${ARDUINO_CORE_MBED_PATH}')")
    elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/libs/arduino-core-mbed)
        set(ARDUINO_CORE_MBED_PATH ${CMAKE_CURRENT_LIST_DIR}/libs/arduino-core-mbed)
        message("Setting ARDUINO_CORE_MBED_PATH from git submodule path ('${ARDUINO_CORE_MBED_PATH}')")
    else()
        message(FATAL_ERROR "ARDUINO_CORE_MBED_PATH not set - run 'git submodule update --init' to fetch the mbed-os-nano submodule")
    endif()
endif()

if (NOT ARDUINO_CORE_API_PATH)
    if (DEFINED ENV{ARDUINO_CORE_API_PATH})
        set(ARDUINO_CORE_API_PATH $ENV{ARDUINO_CORE_API_PATH})
        message("Setting ARDUINO_CORE_API_PATH from environment ('${ARDUINO_CORE_API_PATH}')")
    elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/libs/arduino-core-api)
        set(ARDUINO_CORE_API_PATH ${CMAKE_CURRENT_LIST_DIR}/libs/arduino-core-api)
        message("Setting ARDUINO_CORE_API_PATH from git submodule path ('${ARDUINO_CORE_API_PATH}')")
    else()
        message(FATAL_ERROR "ARDUINO_CORE_API_PATH not set - run 'git submodule update --init' to fetch the arduino-api submodule")
    endif()
endif()

if (NOT WIFININA_PATH)
    if (DEFINED ENV{WIFININA_PATH})
        set(WIFININA_PATH $ENV{WIFININA_PATH})
        message("Setting WIFININA_PATH from environment ('${WIFININA_PATH}')")
    elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/libs/WiFiNINA)
        set(WIFININA_PATH ${CMAKE_CURRENT_LIST_DIR}/libs/WiFiNINA)
        message("Setting WIFININA_PATH from git submodule path ('${WIFININA_PATH}')")
    else()
        message(FATAL_ERROR "WIFININA_PATH not set - run 'git submodule update --init' to fetch the WiFiNINA submodule")
    endif()
endif()

set(PICO_BOARD arduino_nano_rp2040_connect)

#
# Include pico_sdk_import.cmake from pico-sdk
#
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

#
# Generic Setup
#
set(PROJECT skku_bus)
project(${PROJECT} C CXX ASM)

#
# Initialize Pico-SDK
#
pico_sdk_init()

#
# Initialize mbed-os (for Arduino Nano RP2040 Connect)
#
add_library(mbed-os STATIC ${ARDUINO_CORE_MBED_PATH}/cores/arduino)
set_target_properties(mbed-os PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(mbed-os PUBLIC ${ARDUINO_CORE_MBED_PATH}/cores/arduino)

# 
# Language Standards
#
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
target_include_directories(${PROJECT} PRIVATE include ${ARDUINO_CORE_MBED_PATH}/cores/arduino)
target_link_libraries(${PROJECT}
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
    mbed-os
)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-multichar -Wunused")
endif()