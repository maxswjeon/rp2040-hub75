#ifndef _SKKU_BUS_PANEL_H_
#define _SKKU_BUS_PANEL_H_

#include "stdlib.h"
#include "string.h"

#include "pico/stdlib.h"

#include "pincfg.h"

enum PanelPosition
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
    NONE,
};

struct PanelConfig
{
    int rotation;
    PanelPosition next_postion;
};

struct PanelTable
{
    uint32_t min_x;
    uint32_t min_y;
    uint32_t max_x;
    uint32_t max_y;
    uint8_t rotation;
};

class Panel
{
    uint16_t _width;
    uint16_t _height;
    uint8_t _count;

    uint32_t _virtual_width;
    uint32_t _virtual_height;

    uint8_t *_data;

    PanelTable *_panels;
#ifdef _SKKU_BUS_PIO
    uint8_t *_buffer;
#endif

#ifdef _SKKU_BUS_PIO
    PIO _pio;
    uint _sm;
#else
    inline void _shift(uint8_t color1, uint8_t color2);
    inline void _shift(uint8_t color1, uint8_t color2, uint32_t count);
    void _init_gpio();
#endif

public:
#ifdef _SKKU_BUS_PIO
#else
    // Single Panel Constructor
    Panel(uint16_t width, uint16_t height, uint8_t rotation);

    // Multiple Panel Constructor
    Panel(uint16_t width, uint16_t height, uint8_t count, PanelConfig *positions);
#endif

    void setPixel(uint32_t x, uint32_t y, uint8_t color);
    void setPixel(uint32_t x, uint32_t y, bool r, bool g, bool b);
    void clear();

#ifdef _SKKU_BUS_PIO
    void flush();
#else
    void tick();
#endif

    virtual ~Panel();

    void dump();
};

#endif // _SKKU_BUS_PANEL_H_