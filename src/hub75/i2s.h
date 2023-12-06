#ifndef SKKU_BUS_I2S_HPP
#define SKKU_BUS_I2S_HPP

#include <cstddef>
#include <cstdint>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2S_SHIFT_DRIVER_SHIFTREG 0
#define I2S_SHIFT_DRIVER_FM6124 1
#define I2S_SHIFT_DRIVER_FM6126A 2
#define I2S_SHIFT_DRIVER_ICN2038S 3
#define I2S_SHIFT_DRIVER_MBI5124 4
#define I2S_SHIFT_DRIVER_SM5266P 5
#define I2S_SHIFT_DRIVER_DP3246 6
#define I2S_SHIFT_DRIVER_SM5368 6

#define I2S_CLOCK_SPEED_8MHz 8000000
#define I2S_CLOCK_SPEED_10MHz 10000000
#define I2S_CLOCK_SPEED_15MHz 15000000
#define I2S_CLOCK_SPEED_20MHz 20000000

#define I2S_CLOCK_PHASE_NEGATIVE_EDGE false
#define I2S_CLOCK_PHASE_POSTIVE_EDGE true

#define I2S_PINMAP_DEFAULT \
    {                      \
        .R1 = 0,           \
        .G1 = 1,           \
        .B1 = 2,           \
        .R2 = 3,           \
        .G2 = 4,           \
        .B2 = 5,           \
        .A = 6,            \
        .B = 7,            \
        .C = 8,            \
        .D = 9,            \
        .E = 10,           \
        .LAT = 11,         \
        .OE = 12,          \
        .CLK = 13          \
    }

#define COLOR_DEPTH_MAX 12

#define DEFAULT_MATRIX_WIDTH 64
#define DEFAULT_MATRIX_HEIGHT 32
#define DEFAULT_LATCH_BLANKING 2
#define DEFAULT_COLOR_DEPTH 8

namespace HUB75
{
    namespace I2S
    {
        struct Pinmap
        {
            uint8_t R1;
            uint8_t G1;
            uint8_t B1;
            uint8_t R2;
            uint8_t G2;
            uint8_t B2;
            uint8_t A;
            uint8_t B;
            uint8_t C;
            uint8_t D;
            uint8_t E;
            uint8_t LAT;
            uint8_t OE;
            uint8_t CLK;
        };

        struct Config
        {
            const Pinmap _pinmap;

            const uint16_t _width;
            const uint16_t _height;
            const uint16_t _chain_length;

            const uint8_t _driver;
            const bool _double_buffer;
            const uint32_t _clock_speed;
            const uint8_t _latch_blanking;
            const bool _clock_phase;
            const uint8_t _min_refresh_rate;

            Config(
                const Pinmap pinmap = I2S_PINMAP_DEFAULT,
                const uint16_t width = DEFAULT_MATRIX_WIDTH,
                const uint16_t height = DEFAULT_MATRIX_HEIGHT,
                uint16_t chain_length = 1,
                const uint8_t driver = I2S_SHIFT_DRIVER_SHIFTREG,
                bool double_buffer = false,
                uint32_t clock_speed = I2S_CLOCK_SPEED_8MHz,
                uint8_t latch_blanking = DEFAULT_LATCH_BLANKING,
                bool clock_phase = I2S_CLOCK_PHASE_POSTIVE_EDGE,
                uint16_t min_refresh_rate = 60,
                uint8_t depth = DEFAULT_COLOR_DEPTH);

            void setDepth(const uint8_t depth);

        private:
            uint8_t _depth;
        };

    }
}

#endif // SKKU_BUS_I2S_HPP