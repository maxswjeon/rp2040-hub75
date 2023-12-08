#include "Panel.h"
#include "pico/malloc.h"

#include <stdio.h>

static const uint8_t _cho1[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 3, 1, 2, 4, 4, 4, 2, 1, 3, 0};
static const uint8_t _cho2[] = {5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 7, 7, 6, 6, 7, 7, 7, 6, 6, 7, 5};
static const uint8_t _jong[] = {0, 2, 0, 2, 1, 2, 1, 2, 3, 0, 2, 1, 3, 3, 1, 2, 1, 3, 3, 1, 1};

Panel::Panel(uint16_t width, uint16_t height, uint8_t rotation)
{
    _width = width;
    _height = height;
    _count = 1;

    _data = (uint8_t *)malloc(width * height);
    memset(_data, 0, width * _height);

    _font = 0;
    _font_ko = 0;

    _panels = (PanelTable *)malloc(sizeof(PanelTable));
    _panels->min_x = 0;
    _panels->min_y = 0;
    _panels->max_x = width - 1;
    _panels->max_y = height - 1;
    _panels->rotation = rotation;
#ifdef PANEL_PIO
    _buffer = (uint8_t *)malloc(width * height);
#endif

    for (int i = 0; i < width * height; i++)
    {
        _data[i] = 0;

#ifdef PANEL_PIO
        _buffer[i] = 0;
#endif
    }

#ifdef PANEL_PIO
#else
    _init_gpio();
    _shift(0, 0, width);
    tick();
#endif
}

Panel::Panel(uint16_t width, uint16_t height, uint8_t count, const PanelConfig *configs)
{
    _panels = (PanelTable *)malloc(count * sizeof(PanelTable));

    int rotation_scheme = configs[0].rotation % 180;
    int rotation_x = rotation_scheme == 0 ? 64 : 32;
    int rotation_y = rotation_scheme == 0 ? 32 : 64;

    int min_x = 0, cur_x = 0, max_x = rotation_x - 1;
    int min_y = 0, cur_y = 0, max_y = rotation_y - 1;

    for (int i = 0; i < count; ++i)
    {
        struct PanelTable *ti = _panels + count - 1 - i;

        if (rotation_scheme != configs[i].rotation % 180)
        {
            printf("Rotation Scheme Mismatch\n");
            return;
        }

        ti->min_x = cur_x;
        ti->max_x = cur_x + rotation_x - 1;
        ti->min_y = cur_y;
        ti->max_y = cur_y + rotation_y - 1;
        ti->rotation = configs[i].rotation;

        switch (configs[i].next_postion)
        {
        case LEFT:
            cur_x -= rotation_x;
            min_x = cur_x < min_x ? cur_x : min_x;
            break;
        case RIGHT:
            cur_x += rotation_x;
            max_x = cur_x > max_x ? cur_x : max_x;
            break;
        case UP:
            cur_y -= rotation_y;
            min_y = cur_y < min_y ? cur_y : min_y;
            break;
        case DOWN:
            cur_y += rotation_y;
            max_y = cur_y > max_y ? cur_y : max_y;
            break;
        }
    }

    for (int i = 0; i < count; ++i)
    {
        struct PanelTable *ti = _panels + i;
        ti->min_x -= min_x;
        ti->max_x -= min_x;
        ti->min_y -= min_y;
        ti->max_y -= min_y;
    }

    _width = width;
    _height = height;

    _count = count;

    // Virtual Coordinate Space
    _data = (uint8_t *)malloc(_width * _height * count);

    _font = 0;
    _font_ko = 0;

#ifdef PANEL_PIO
    _buffer = (uint8_t *)malloc(width * height);
#endif

    for (int i = 0; i < width * height; i++)
    {
        _data[i] = 0;

#ifdef PANEL_PIO
        _buffer[i] = 0;
#endif
    }

#ifdef PANEL_PIO
#else
    _init_gpio();
    _shift(0, 0, width * count);
    tick();
#endif
}

void Panel::_init_gpio()
{
    gpio_init(R0);
    gpio_init(G0);
    gpio_init(B0);
    gpio_init(R1);
    gpio_init(G1);
    gpio_init(B1);
    gpio_init(A);
    gpio_init(B);
    gpio_init(C);
    gpio_init(D);
    gpio_init(CLK);
    gpio_init(OE);
    gpio_init(LAT);

    gpio_set_dir(R0, GPIO_OUT);
    gpio_set_dir(G0, GPIO_OUT);
    gpio_set_dir(B0, GPIO_OUT);
    gpio_set_dir(R1, GPIO_OUT);
    gpio_set_dir(G1, GPIO_OUT);
    gpio_set_dir(B1, GPIO_OUT);
    gpio_set_dir(A, GPIO_OUT);
    gpio_set_dir(B, GPIO_OUT);
    gpio_set_dir(C, GPIO_OUT);
    gpio_set_dir(D, GPIO_OUT);
    gpio_set_dir(CLK, GPIO_OUT);
    gpio_set_dir(OE, GPIO_OUT);
    gpio_set_dir(LAT, GPIO_OUT);
}

#ifdef PANEL_PIO
#else
inline void Panel::_shift(uint8_t color1, uint8_t color2)
{
    gpio_put(R0, (color1 & 0b100) >> 2);
    gpio_put(G0, (color1 & 0b010) >> 1);
    gpio_put(B0, (color1 & 0b001) >> 0);
    gpio_put(R1, (color2 & 0b100) >> 2);
    gpio_put(G1, (color2 & 0b010) >> 1);
    gpio_put(B1, (color2 & 0b001) >> 0);
    gpio_put(CLK, 1);
    sleep_us(1);
    gpio_put(CLK, 0);
    sleep_us(1);
}

inline void Panel::_shift(uint8_t color1, uint8_t color2, uint32_t count)
{
    gpio_put(R0, (color1 & 0b100) >> 2);
    gpio_put(G0, (color1 & 0b010) >> 1);
    gpio_put(B0, (color1 & 0b001) >> 0);
    gpio_put(R1, (color2 & 0b100) >> 2);
    gpio_put(G1, (color2 & 0b010) >> 1);
    gpio_put(B1, (color2 & 0b001) >> 0);
    for (int i = 0; i < count; i++)
    {
        gpio_put(CLK, 1);
        sleep_us(1);
        gpio_put(CLK, 0);
        sleep_us(1);
    }
}
#endif

void Panel::setPixel(uint32_t x, uint32_t y, uint8_t color)
{
    PanelTable *panel = _panels;
    for (; panel < _panels + _count; ++panel)
    {
        // Panel Table Hit
        if (panel->min_x <= x && panel->max_x >= x && panel->min_y <= y && panel->max_y >= y)
        {
            break;
        }
    }

    // Out of Bounds
    if (panel - _panels == _count)
    {
        return;
    }

    int index = panel - _panels;
    uint32_t px, py;
    // Rotation consistency is assumed - (0, 180), (90, 270) is paired
    if (panel->rotation == 0)
    {
        px = index * _width + (x % _width);
        py = (y % _height);
    }
    else if (panel->rotation == 180)
    {
        px = index * _width + (_width - 1 - (x % _width));
        py = _height - 1 - (y % _height);
    }
    else if (panel->rotation == 90)
    {
        px = index * _width + (_height - 1 - (y % _height));
        py = (x % _width);
    }
    else if (panel->rotation == 270)
    {
        px = index * _width + (y % _height);
        py = _width - 1 - (x % _width);
    }

    _data[py * _width * _count + px] = color;
}

void Panel::setPixel(uint32_t x, uint32_t y, bool r, bool g, bool b)
{
    setPixel(x, y, ((r ? 1 : 0) << 2) | ((g ? 1 : 0) << 1) | ((b ? 1 : 0) << 0));
}

void Panel::clear()
{
    for (int i = 0; i < _width * _height * _count; i++)
    {
#ifdef PANEL_PIO
        _buffer[i] = 0;
#else
        _data[i] = 0;
#endif
    }
}

void Panel::setFont(const uint8_t *font, const uint8_t *font_ko)
{
    _font = font;
    _font_ko = font_ko;
}

int Panel::printChar(int32_t x, int32_t y, uint32_t c, uint8_t color)
{
    if (c >= 0x20 && c <= 0x7E)
    {
        if (!_font)
        {
            printf("English Font not set\n");
            return 0;
        }

        for (uint32_t i = 0; i < 8 * 16; ++i)
        {
            if (_font[(c - 0x20) * 16 + i / 8] & (1 << (7 - (i % 8))))
            {
                setPixel(x + (i % 8), y + (i / 8), color);
            }
        }
        return 8;
    }
    else if (c >= 0xAC00 && c <= 0xD7A3)
    {
        if (!_font_ko)
        {
            printf("Korean not set\n");
            return 0;
        }

        uint8_t index_cho = (c - 0xAC00) / (21 * 28);
        uint8_t index_jung = ((c - 0xAC00) % (21 * 28)) / 28;
        uint8_t index_jong = (c - 0xAC00) % 28;

        if (index_jong == 0)
        {
            uint8_t cho_index0 = 0;
            uint8_t cho_index1 = _cho1[index_jung];
            uint8_t cho_index2 = index_cho;

            uint8_t jung_index0 = 19 * 8;
            uint8_t jung_index1 = index_cho == 1 || index_cho == 24 ? 0 : 1;
            uint8_t jung_index2 = index_jung;

            uint32_t cho = cho_index0 + 19 * cho_index1 + cho_index2;
            uint32_t jung = jung_index0 + 21 * jung_index1 + jung_index2;

            for (uint32_t i = 0; i < 16 * 16; ++i)
            {
                uint8_t flag = (_font_ko[cho * 32 + i / 8] | _font_ko[jung * 32 + i / 8]) & (1 << (7 - (i % 8))) ? 1 : 0;
                setPixel(x + (i % 16), y + (i / 16), flag * color);
            }
        }
        else
        {
            uint8_t cho_index0 = 0;
            uint8_t cho_index1 = _cho2[index_jung];
            uint8_t cho_index2 = index_cho;

            uint8_t jung_index0 = 19 * 8;
            uint8_t jung_index1 = index_cho == 1 || index_cho == 24 ? 2 : 3;
            uint8_t jung_index2 = index_jung;

            uint8_t jong_index0 = 19 * 8 + 21 * 4;
            uint8_t jong_index1 = _jong[index_jung];
            uint8_t jong_index2 = index_jong - 1;

            uint32_t cho = cho_index0 + 19 * cho_index1 + cho_index2;
            uint32_t jung = jung_index0 + 21 * jung_index1 + jung_index2;
            uint32_t jong = jong_index0 + 27 * jong_index1 + jong_index2;

            for (uint32_t i = 0; i < 16 * 16; ++i)
            {
                uint8_t flag = (_font_ko[cho * 32 + i / 8] | _font_ko[jung * 32 + i / 8] | _font_ko[jong * 32 + i / 8]) & (1 << (7 - (i % 8))) != 0 ? 1 : 0;
                setPixel(x + (i % 16), y + (i / 16), flag * color);
            }
        }
        return 16;
    }
    else
    {
        printf("Out of Range Character!\n");
        if (!_font)
        {
            printf("English Font not set\n");
            return 0;
        }

        c = 0x3F;
        for (int i = 0; i < 8 * 16; ++i)
        {
            uint8_t flag = _font[(c - 0x20) * 16 + i / 8] & (1 << (7 - (i % 8))) ? 1 : 0;
            setPixel(x + (i % 4), y + (i / 4), flag * color);
        }
        return 8;
    }
}

void Panel::printString(int32_t x, int32_t y, const char *str, uint8_t color)
{
    uint32_t cur_x = x;
    uint32_t cur_y = y;

    while (*str)
    {
        if (*str == '\n')
        {
            cur_x = x;
            cur_y += 16;
        }
        else
        {
            cur_x += printChar(cur_x, cur_y, *str, color);
        }
        ++str;
    }
}

#ifdef PANEL_PIO
void Panel::flush()
{
    uint8_t *temp = _data;
    _data = _buffer;
    _buffer = temp;
}
#else
void Panel::tick()
{
    int half = _height / 2;

    for (uint32_t y = 0; y < half; ++y)
    {
        for (uint32_t x = 0; x < _width * _count; ++x)
        {
            _shift(_data[y * _width * _count + x], _data[(y + half) * _width * _count + x]);
        }
        gpio_put(OE, 1);
        gpio_put(LAT, 1);
        sleep_us(1);
        gpio_put(LAT, 0);
        gpio_put(A, (y & 0b0001) >> 0);
        gpio_put(B, (y & 0b0010) >> 1);
        gpio_put(C, (y & 0b0100) >> 2);
        gpio_put(D, (y & 0b1000) >> 3);
        gpio_put(OE, 0);
    }
}
#endif

Panel::~Panel()
{
    free(_panels);

    free(_data);

#ifdef PANEL_PIO
    free(_buffer);
#endif
}