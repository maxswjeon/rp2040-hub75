#ifndef _SKKU_BUS_PANEL_H_
#define _SKKU_BUS_PANEL_H_

#include <stdlib.h>
#include <string.h>

#include "pincfg.h"

#include "pico/stdlib.h"

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

    uint8_t *_data;

    PanelTable *_panels;

    const uint8_t *_font;
    const uint8_t *_font_ko;
#ifdef PANEL_PIO
    uint8_t *_buffer;
#endif

#ifdef PANEL_PIO
#else
    inline void _shift(uint8_t color1, uint8_t color2);
    inline void _shift(uint8_t color1, uint8_t color2, uint32_t count);
    void _init_gpio();
#endif

public:
#ifdef PANEL_PIO
#else
    // Single Panel Constructor
    Panel(uint16_t width, uint16_t height, uint8_t rotation);

    // Multiple Panel Constructor
    Panel(uint16_t width, uint16_t height, uint8_t count, const PanelConfig *positions);
#endif

    void setPixel(uint32_t x, uint32_t y, uint8_t color);
    void setPixel(uint32_t x, uint32_t y, bool r, bool g, bool b);
    void clear();

    void setFont(const uint8_t *font, const uint8_t *font_ko);
    int printChar(int32_t x, int32_t y, uint32_t c, uint8_t color = 0xFF);
    void printString(int32_t x, int32_t y, const wchar_t *str, uint8_t color = 0xFF);
    void printString(int32_t x, int32_t y, const char *str, uint8_t color = 0xFF);
#ifdef PANEL_PIO
    void flush();
#else
    void tick();
#endif
    virtual ~Panel();
};

#endif // _SKKU_BUS_PANEL_H_