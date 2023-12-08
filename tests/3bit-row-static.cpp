#include <stdio.h>
#include "pico/stdlib.h"

#include "pincfg.h"

#define PANNEL_WIDTH 64

void init_gpio()
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

void drawLine(uint8_t row, bool r, bool g, bool b)
{
    gpio_put(R0, r);
    gpio_put(G0, g);
    gpio_put(B0, b);
    gpio_put(R1, r);
    gpio_put(G1, g);
    gpio_put(B1, b);
    for (int i = 0; i < PANNEL_WIDTH; i++)
    {
        gpio_put(CLK, 1);
        sleep_us(1);
        gpio_put(CLK, 0);
        sleep_us(1);
    }
    gpio_put(OE, 1);
    gpio_put(6, 0);
    gpio_put(LAT, 1);
    sleep_us(1);
    gpio_put(LAT, 0);

    gpio_put(A, (row & 0b0001) >> 0);
    gpio_put(B, (row & 0b0010) >> 1);
    gpio_put(C, (row & 0b0100) >> 2);
    gpio_put(D, (row & 0b1000) >> 3);

    gpio_put(OE, 0);
    gpio_put(6, 1);
}

int main()
{
    stdio_init_all();
    init_gpio();
    gpio_init(6);
    gpio_set_dir(6, GPIO_OUT);

    while (true)
    {
        for (int i = 0; i < 32; ++i)
        {
            drawLine(i, (i & 0b100) >> 2, (i & 0b010) >> 1, (i & 0b001) >> 0);
        }
    }
}