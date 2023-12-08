#include "Panel.h"
#include "pico/malloc.h"

#include <stdio.h>

Panel::Panel(uint16_t width, uint16_t height, uint8_t rotation)
{
    _virtual_width = width;
    _virtual_height = height;

    _width = width;
    _height = height;
    _count = 1;

    _data = (uint8_t *)malloc(width * height);
    memset(_data, 0, _width * _height);

    _panels = (PanelTable *)malloc(sizeof(PanelTable));
    _panels->min_x = 0;
    _panels->min_y = 0;
    _panels->max_x = width - 1;
    _panels->max_y = height - 1;
    _panels->rotation = rotation;
#ifdef _SKKU_BUS_PIO
    _buffer = (uint8_t *)malloc(width * height);
#endif

    for (int i = 0; i < width * height; i++)
    {
        _data[i] = 0;

#ifdef _SKKU_BUS_PIO
        _buffer[i] = 0;
#endif
    }

#ifdef _SKKU_BUS_PIO
#else
    _init_gpio();
    _shift(0, 0, 64);
    tick();
#endif
}

Panel::Panel(uint16_t width, uint16_t height, uint8_t count, PanelConfig *configs)
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

    _virtual_width = max_x - min_x;
    _virtual_height = max_y - min_y;
    _width = width;
    _height = height;

    _count = count;

    // Virtual Coordinate Space
    _data = (uint8_t *)malloc(_width * _height * count);
    memset(_data, 0, _width * _height * count);
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

#ifdef _SKKU_BUS_PIO
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
    sleep_us(2);
    gpio_put(CLK, 0);
    sleep_us(2);
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
        sleep_us(2);
        gpio_put(CLK, 0);
        sleep_us(2);
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
        printf("Out of Bounds\n");
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

    printf("Set Pixel: (%d, %d) -> (%d, %d)\n", x, y, px, py);
}

void Panel::setPixel(uint32_t x, uint32_t y, bool r, bool g, bool b)
{
    setPixel(x, y, ((r ? 1 : 0) << 2) | ((g ? 1 : 0) << 1) | ((b ? 1 : 0) << 0));
}

void Panel::clear()
{
    for (int i = 0; i < _virtual_width * _virtual_height; i++)
    {
#ifdef _SKKU_BUS_PIO
        _buffer[i] = 0;
#else
        _data[i] = 0;
#endif
    }
}

#ifdef _SKKU_BUS_PIO
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

    for (int y = 0; y < half; ++y)
    {
        for (int x = 0; x < _width * _count; ++x)
        {
            _shift(_data[y * _width * _count + x], _data[(y + half) * _width * _count + x]);
        }
        gpio_put(OE, 1);
        gpio_put(LAT, 1);
        sleep_us(2);
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

#ifdef _SKKU_BUS_PIO
    free(_buffer);
#endif
}

void Panel::dump()
{
    for (int y = 0; y < _height; ++y)
    {
        for (int x = 0; x < _width * _count; ++x)
        {
            printf("%d ", _data[y * _width * _count + x]);
            if (x % _width == _width - 1)
                printf("| ");
        }
        printf("\n");
    }

    printf("\n");
}