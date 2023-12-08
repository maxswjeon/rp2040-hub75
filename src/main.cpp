#include <stdio.h>
#include "pico/stdlib.h"
#include "pincfg.h"
#include "Panel.h"

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32

int main()
{
    uint64_t lastblink = 0;
    bool blink = false;

    PanelConfig configs[4] = {
        {0, LEFT},
        {0, UP},
        {180, RIGHT},
        {180, NONE},
    };

    stdio_init_all();

    Panel panel(PANEL_WIDTH, PANEL_HEIGHT, 4, configs);
    sleep_ms(5000);

    panel.setPixel(0, 0, 1);
    panel.setPixel(64, 0, 1);
    panel.setPixel(0, 32, 1);
    panel.setPixel(64, 32, 1);

    panel.dump();

    while (true)
    {
        if (time_us_64() - lastblink > 500 * 1000)
        {
            gpio_init(6);
            gpio_set_dir(6, GPIO_OUT);
            blink = !blink;
            gpio_put(6, blink);
            lastblink = time_us_64();
        }

        panel.tick();
    }
}