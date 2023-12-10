#include "Serial.h"

using namespace arduino;

UART::UART() {}
UART::UART(int tx, int rx, int rts, int cts)
{
    gpio_init(tx);
    gpio_set_dir(tx, GPIO_OUT);
    gpio_init(rx);
    gpio_set_dir(rx, GPIO_IN);
    gpio_pull_up(rx);

    if (rts != -1 && cts != -1)
    {
        gpio_init(rts);
        gpio_set_dir(rts, GPIO_OUT);
        gpio_init(cts);
        gpio_set_dir(cts, GPIO_IN);
    }

    _tx = tx;
    _rx = rx;
    _rts = rts;
    _cts = cts;
}