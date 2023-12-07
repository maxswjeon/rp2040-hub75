#ifndef _SKKU_BUS_PANEL_H_
#define _SKKU_BUS_PANEL_H_

#include "pico/stdlib.h"

enum PanelRotation
{
    ROTATION_0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270
};

enum PanelRelativePosition
{
    TOPLEFT,
    TOPCENTER,
    TOPRIGHT,
    CENTERLEFT,
    CONFLICT,
    CENTERRIGHT,
    BOTTOMLEFT,
    BOTTOMCENTER,
    BOTTOMRIGHT,
};

class Panel
{
    uint8_t *_data;
    uint8_t *_buffer;

    void swap();

public:
    // Single Panel Constructor
    Panel(int width, int height, PanelRotation rotation);

    // Multiple Panel Constructor
    Panel(int width, int height, int count, PanelRotation *rotations, PanelRelativePosition *positions);

    void setPixel(int x, int y, uint8_t color);
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void flush();

    virtual ~Panel();
};

#endif // _SKKU_BUS_PANEL_H_