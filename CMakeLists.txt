cmake_minimum_required(VERSION 3.25)

if (NOT PICO_SDK_PATH)
    if(DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
        message("Setting PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
    else()
        set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/libs/pico-sdk)
        message("Setting PICO_SDK_PATH from git submodule path ('${PICO_SDK_PATH}')")
    endif()
else()
    message("PICO_SDK_PATH already set ('${PICO_SDK_PATH}')")
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
# Language Standards
#
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-multichar -Wunused")
endif()