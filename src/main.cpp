#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32
// #define PANEL_PIO

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/flash.h"

#include "pincfg.h"
#include "hub75/Panel.h"
#include "html/html.hpp"
#include "flash.h"
#include "Neodgm.h"

#include "WiFi.h"

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

#define SSID_PREFIX "SKKU-"
static_assert(strlen(SSID_PREFIX) <= 7, "SSID_PREFIX must be less than 6 characters");

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

void init_flash()
{
    sleep_ms(3000);
    panel->setFont(Neodgm, NeodgmKorean);

    panel->printString(0, -3, "Initializing...");
    panel->printString(0, 10, "[ ] Flash Check");
    panel->printString(0, 29, "[ ] Flash Init");

    printf("Flash Check\n");
    printf("    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F | \n");
    for (int i = 0; i < FLASH_SECTOR_SIZE / 16; ++i)
    {
        printf("%02X: ", i);
        for (int j = 0; j < 16; ++j)
        {
            printf("%02X  ", flash_target_contents[i * 16 + j]);
        }
        for (int j = 0; j < 16; ++j)
        {
            printf("%c", flash_target_contents[i * 16 + j] < 0x20 || flash_target_contents[i * 16 + j] > 0x7E ? '.' : flash_target_contents[i * 16 + j]);
        }
        printf("\n");
    }
    if (flash_read_uint32(flash_target_contents, 0) == FLASH_CHECKSUM)
    {
        panel->printChar(8, 8, 'v');
        panel->printChar(8, 28, 's');
    }
    else
    {
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

        panel->setPixel(8, 36, 0xFF);

        memset(flash_target_buffer, 0, FLASH_SECTOR_SIZE);
        flash_target_buffer[0] = 'S';
        flash_target_buffer[1] = 'K';
        flash_target_buffer[2] = 'K';
        flash_target_buffer[3] = 'U';
        for (int i = 0; i < FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE; ++i)
        {
            panel->setPixel(8 + (i + 1) % 8, 36, 0xFF);
            flash_range_program(FLASH_TARGET_OFFSET + i * FLASH_PAGE_SIZE, flash_target_buffer + i * FLASH_PAGE_SIZE, FLASH_PAGE_SIZE);
        }

        panel->printChar(8, 28, 'v');
    }

    sleep_ms(1000);
    panel->clear();
}

static const PanelConfig configs[4] = {
    {0, LEFT},
    {0, UP},
    {180, RIGHT},
    {180, NONE},
};

void setup_wifi()
{
    char ssid[17];
    char psk[17];

    uint16_t ssid_postfix = rand() % 10000;
    sprintf(ssid, "%s%04d", SSID_PREFIX, ssid_postfix);
    sprintf(psk, "%08d", rand() % 100000000);

    panel->printString(0, 0, "Connect to WiFi");
    panel->printString(0, 16, "to configure.");
    panel->printString(0, 32, "SSID:");
    panel->printString(40, 32, ssid);
    panel->printString(0, 48, "PSK: ");
    panel->printString(40, 48, psk);

    bool result = wifi_set_ap(ssid, strlen(ssid), 1);
    printf("WiFi Setup Result: %d\n", result);
}

void connect_wifi()
{
    uint32_t ssid_offset = flash_read_uint32(flash_target_contents, 0x04);
    uint32_t ssid_length = flash_read_uint32(flash_target_contents, 0x08);
    uint32_t psk_offset = flash_read_uint32(flash_target_contents, 0x0C);
    uint32_t psk_length = flash_read_uint32(flash_target_contents, 0x10);

    printf("SSID Offset: %d, Length: %d\n", ssid_offset, ssid_length);
    printf("PSK Offset: %d, Length: %d\n", psk_offset, psk_length);

    if (ssid_length == 0)
    {
        setup_wifi();
        while (true)
            ;
    }
}

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

    // gpio_init(D13);
    // gpio_init(D14);
    // gpio_set_dir(D14, GPIO_IN);
    // gpio_pull_up(D13);
    // gpio_pull_up(D14);

    // if (!gpio_get(D13))
    // {
    //     bluescreen();
    //     while (true)
    //         ;
    // }

    init_flash();

    wifi_init();
    wifi_gpio_init(LEDR);
    wifi_gpio_set_dir(LEDR, OUTPUT);
    wifi_gpio_put(LEDR, 1);

    connect_wifi();
}