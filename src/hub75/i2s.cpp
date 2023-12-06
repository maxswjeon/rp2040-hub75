#include "i2s.h"

using namespace HUB75::I2S;

Config::Config(
    const Pinmap pinmap,
    const uint16_t width,
    const uint16_t height,
    uint16_t chain_length,
    const uint8_t driver,
    bool double_buffer,
    uint32_t clock_speed,
    uint8_t latch_blanking,
    bool clock_phase,
    uint16_t min_refresh_rate,
    uint8_t depth) : _pinmap(pinmap),
                     _width(width),
                     _height(height),
                     _chain_length(chain_length),
                     _driver(driver),
                     _double_buffer(double_buffer),
                     _clock_speed(clock_speed),
                     _latch_blanking(latch_blanking),
                     _clock_phase(clock_phase),
                     _min_refresh_rate(min_refresh_rate)
{
    setDepth(depth);
}

void Config::setDepth(const uint8_t depth)
{
    _depth = depth;
    if (_depth > COLOR_DEPTH_MAX)
    {
        _depth = COLOR_DEPTH_MAX;
    }
    else if (_depth < 2)
    {
        _depth = 2;
    }
}