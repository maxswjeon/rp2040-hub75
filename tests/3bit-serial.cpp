#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "pincfg.h"

#include "Panel.h"

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 33

#define INPUT_NOTHING 1
#define INPUT_NEXTVAR 2
#define INPUT_NEWLINE 3
#define INPUT_ERROR -255

Panel *panel = 0;

void driver()
{
    while (true)
    {
        if (panel)
        {
            panel->tick();
        }
    }
}

int handle_input(uint8_t *next)
{
    int c = getchar();
    if (c == PICO_ERROR_TIMEOUT || c == ' ')
    {
        return INPUT_NOTHING;
    }
    else if (c == '\n')
    {
        return INPUT_NEWLINE;
    }
    else if (c == ',')
    {
        return INPUT_NEXTVAR;
    }
    else if (c > '0' || c < '9')
    {
        *next *= 10;
        *next += c - '0';
        return INPUT_NOTHING;
    }
    return INPUT_ERROR;
}

int main()
{
    gpio_init(6);
    gpio_set_dir(6, GPIO_OUT);
    gpio_put(6, 1);

    panel = new Panel(PANEL_WIDTH, PANEL_HEIGHT, ROTATION_0);

    stdio_init_all();

    multicore_launch_core1(&driver);
    printf("Driver started\n");

    getchar_timeout_us(0);

    uint8_t data[5] = {
        0,
    };
    uint8_t data_index = 0;

    while (true)
    {
        gpio_put(6, 1);

        int result = handle_input(data + data_index);
        bool error = false;

        if (result == INPUT_ERROR)
        {
            printf("Invalid Input\n");
            error = true;
        }
        else if (result == INPUT_NEXTVAR)
        {
            if (data_index == 4)
            {
                printf("Invalid Input - Too many variables\n");
                error = true;
            }
            else if (data_index == 0)
            {
                if (data[0] < 0 || data[0] >= PANEL_WIDTH)
                {
                    printf("Invalid Input - X out of range\n");
                    printf("Expected: 0 ~ %d, got %d\n", PANEL_WIDTH - 1, data[0]);
                    error = true;
                }
            }
            else if (data_index == 1)
            {
                if (data[1] < 0 || data[1] >= PANEL_HEIGHT)
                {
                    printf("Invalid Input - Y out of range\n");
                    printf("Expected: 0 ~ %d, got %d\n", PANEL_HEIGHT - 1, data[0]);
                    error = true;
                }
            }
            else if (data[data_index] != 0 && data[data_index] != 1)
            {
                const char *color = data_index == 2 ? "Red" : data_index == 3 ? "Green"
                                                                              : "Blue";
                printf("Invalid Input - Invalid %s value (should be 0 or 1)\n", color);
                printf("Excepted: 0 or 1, got %d\n", data[data_index]);
                error = true;
            }

            if (!error)
            {
                data_index++;
            }
        }
        else if (result == INPUT_NEWLINE && data_index == 4)
        {
            panel->setPixel(data[0], data[1], data[2] == 1, data[3] == 1, data[4] == 1);
            printf("Set Pixel (%d, %d) to (%d, %d, %d)\n", data[0], data[1], data[2], data[3], data[4]);
            data_index = 0;
            data[0] = 0;
            data[1] = 0;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
        }

        if (error)
        {
            data_index = 0;
            data[0] = 0;
            data[1] = 0;
            data[2] = 0;
            data[3] = 0;
            data[4] = 0;
            while (getchar() != PICO_ERROR_TIMEOUT)
                ;
        }
    }
}