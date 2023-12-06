#ifndef SKKU_BUS_PANEL_H
#define SKKU_BUS_PANEL_H

#include <vector>
#include <memory>

#include "i2s.h"

namespace HUB75
{
    class Panel
    {
    public:
        Panel();
        Panel(const I2S::Config config);
        virtual ~Panel();
        bool begin();

        // Fast Functions
        //

        void fillScreenRGB888(const uint8_t r, const uint8_t g, const uint8_t b);
        void drawPixelRGB888(const uint16_t x, const uint16_t y, const uint8_t r, const uint8_t g, const uint8_t b);

        // GFX_ROOT
        //

        void drawIcon(int *icon, int x, int y, int w, int h);
        static uint16_t color333(uint8_t r, uint8_t g, uint8_t b);
        static uint16_t color444(uint8_t r, uint8_t g, uint8_t b);
        static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
        static void color565to888(const uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b);

        inline void flipDMABuffer();
    };
}

#endif // SKKU_BUS_PANEL_H