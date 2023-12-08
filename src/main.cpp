#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32
// #define PANEL_PIO

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/flash.h"

#include "pincfg.h"
#include "Panel.h"
#include "html.hpp"
#include "flash.h"
#include "Neodgm.h"

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

uint8_t flash_target_buffer[FLASH_SECTOR_SIZE];
const uint8_t *flash_target_contents = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

Panel *panel;

void bluescreen()
{
    panel->setFont(Neodgm, NeodgmKorean);

    panel->printString(0, -3, "A problem has be");
    panel->printString(0, 10, "to your computer.");
    panel->printString(0, 29, "If this is the f");
    panel->printString(0, 41, "restart your com");
    panel->printString(0, 54, "these steps:");
}

static const PanelConfig configs[4] = {
    {0, LEFT},
    {0, UP},
    {180, RIGHT},
    {180, NONE},
};

void driver()
{
    uint64_t lastblink = 0;
    bool blink = false;

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

        panel->tick();
    }
}

int main()
{
    stdio_init_all();

    // Initialize the Panel
    panel = new Panel(PANEL_WIDTH, PANEL_HEIGHT, 4, configs);
    panel->clear();

    multicore_launch_core1(&driver);

    gpio_init(D13);
    // gpio_init(D14);
    gpio_set_dir(D13, GPIO_IN);
    // gpio_set_dir(D14, GPIO_IN);
    gpio_pull_up(D13);
    // gpio_pull_up(D14);

    if (gpio_get(D13) == 0)
    {
        bluescreen();
        while (true)
            ;
    }

    // // Check if the flash is initialized
    // if (flash_read_uint32(flash_target_contents, 0) != FLASH_CHECKSUM)
    // {
    //     panel->printChar(0, 0, 'F', 7);

    //     // flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    //     memset(flash_target_buffer, 0, FLASH_SECTOR_SIZE);
    //     flash_target_buffer[0] = 'S';
    //     flash_target_buffer[1] = 'K';
    //     flash_target_buffer[2] = 'K';
    //     flash_target_buffer[3] = 'U';
    //     for (int i = 0; i < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; ++i)
    //     {
    //         // flash_range_program(FLASH_TARGET_OFFSET + (i * FLASH_PAGE_SIZE), flash_target_buffer + (i * FLASH_PAGE_SIZE), FLASH_PAGE_SIZE);
    //     }
    // }
    // else
    // {
    //     panel->printChar((const uint8_t **)Neodgm, (const uint8_t **)NeodgmKorean, 0, 0, 'V', 7);
    // }
}