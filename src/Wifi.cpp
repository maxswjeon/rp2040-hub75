#include "WiFi.h"
#include <stdio.h>

#define CMD_FLAG 0
#define REPLY_FLAG 1 << 7
#define DATA_FLAG 0x40

#define WIFI_SPI_ACK 1
#define WIFI_SPI_ERR 0xFF

#define START_CMD 0xE0
#define END_CMD 0xEE
#define ERR_CMD 0xEF

#define DUMMY_DATA 0xFF

#define READ_TIMEOUT 1000

#define LAST_STATE_NONE 0x00
#define LAST_STATE_READ 0x01
#define LAST_STATE_WRITE 0x02

bool _transaction_started = false;

uint8_t _last_read = LAST_STATE_NONE;
uint8_t _transaction_count = 0;

void _spi_write(uint8_t data)
{
    spi_write_blocking(SPI_CHANNEL, &data, 1);
    if (_last_read != LAST_STATE_WRITE)
    {
        _transaction_count = 0;
        printf("\n\nW\n");
    }
    printf("%02X ", data);
    if (++_transaction_count % 16 == 0)
        printf("\n");
    _last_read = LAST_STATE_WRITE;
}

uint8_t _spi_read()
{
    uint8_t cmd;
    spi_read_blocking(SPI_CHANNEL, DUMMY_DATA, &cmd, 1);

    if (_last_read != LAST_STATE_READ)
    {
        _transaction_count = 0;
        printf("\n\nR\n");
    }
    printf("%02X ", cmd);
    if (++_transaction_count % 16 == 0)
        printf("\n");
    _last_read = LAST_STATE_READ;

    return cmd;
}

void _send_command(uint8_t command, uint8_t argc = 0)
{
    _spi_write(START_CMD);
    _spi_write(command | CMD_FLAG);
    _spi_write(argc);
}

void _send_param(const uint8_t *param, uint8_t len)
{
    _spi_write(len);
    for (int i = 0; i < len; ++i)
    {
        _spi_write(param[i]);
    }
}

uint8_t _get_param(uint8_t *param_len)
{
    uint8_t len = _spi_read();
    if (param_len)
    {
        *param_len = len;
    }

    return len;
}

bool _expect(uint8_t expected, uint8_t *actual)
{
    _get_param(actual);
    if (*actual != expected)
    {
        return false;
    }
    return true;
}

void _end_command()
{
    _spi_write(END_CMD);
}

bool _wait_char(uint8_t c)
{
    uint32_t timeout = READ_TIMEOUT;
    uint8_t _readChar = 0;

    do
    {
        _readChar = _spi_read();
        if (_readChar == ERR_CMD)
        {
            return false;
        }
    } while (timeout-- > 0 && _readChar != c);

    return _readChar == c;
}

bool _wait_response(uint8_t cmd, uint8_t argc, uint8_t *argv, uint8_t *argl)
{
    uint8_t _data = 0;
    int i = 0;

    if (!_wait_char(START_CMD))
    {
        return false;
    }

    if (!_expect(cmd | REPLY_FLAG, &_data))
    {
        return false;
    }

    if (!_expect(argc, &_data))
    {
        return false;
    }

    _get_param(argl);
    for (int i = 0; i < *argl; ++i)
    {
        _get_param(&argv[i]);
    }
    _expect(END_CMD, &_data);

    return true;
}

void _wait_ready()
{
    while (gpio_get(SPI_ACK_PIN) != 0)
        ;
}

void wifi_init()
{
    spi_inst_t *spi = SPI_CHANNEL;

    spi_init(spi, 8 * 1000 * 1000);
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CSn_PIN, GPIO_FUNC_SPI);

    gpio_init(SPI_RESET_PIN);
    gpio_set_dir(SPI_RESET_PIN, GPIO_OUT);
    gpio_put(SPI_RESET_PIN, 0);
    sleep_ms(1000);
    gpio_put(SPI_RESET_PIN, 1);

    gpio_init(SPI_ACK_PIN);
    gpio_set_dir(SPI_ACK_PIN, GPIO_IN);

    _wait_ready();
}

bool wifi_set_ap(const char *ssid, uint8_t ssid_len, uint8_t channel)
{
    _send_command(SET_AP_NET_CMD, 2);
    _send_param((const uint8_t *)ssid, ssid_len);
    _send_param(&channel, 1);
    _end_command();

    int command_size = 3 + ssid_len;
    while (command_size % 4)
    {
        _spi_write(DUMMY_DATA);
        command_size++;
    }

    _wait_ready();

    uint8_t data = 0, data_len = 0;
    if (!_wait_response(SET_AP_NET_CMD, 1, &data, &data_len))
    {
        data = 0xFF;
    }

    return data == WIFI_SPI_ACK;
}

bool wifi_set_ap(const char *ssid, uint8_t ssid_len, const char *psk, uint8_t psk_len, uint8_t channel)
{
    _send_command(SET_AP_PASSPHRASE_CMD, 3);
    _send_param((const uint8_t *)ssid, ssid_len);
    _send_param((const uint8_t *)psk, psk_len);
    _send_param(&channel, 1);
    _end_command();
    int command_size = 4 + ssid_len + psk_len;
    while (command_size % 3)
    {
        _spi_write(DUMMY_DATA);
        command_size++;
    }

    _wait_ready();

    uint8_t data = 0, data_len = 0;
    if (!_wait_response(SET_AP_PASSPHRASE_CMD, 1, &data, &data_len))
    {
        data = 0xFF;
    }

    return data == WIFI_SPI_ACK;
}

bool wifi_gpio_init(uint8_t pin)
{
    // skip
    return true;
}

bool wifi_gpio_set_dir(uint8_t pin, uint8_t dir)
{
    _send_command(SET_PIN_MODE, 2);
    _send_param(&pin, 1);
    _send_param(&dir, 1);
    _end_command();
    _spi_read(); // pad data

    _wait_ready();

    uint8_t data = 0, data_len = 0;
    if (!_wait_response(SET_PIN_MODE, 1, &data, &data_len))
    {
        data = 0xFF;
    }

    return data == WIFI_SPI_ACK;
}

bool wifi_gpio_put(uint8_t pin, uint8_t value)
{
    _send_command(SET_DIGITAL_WRITE, 2);
    _send_param(&pin, 1);
    _send_param(&value, 1);
    _end_command();
    _spi_read(); // pad data

    _wait_ready();

    uint8_t data = 0, data_len = 0;
    if (!_wait_response(SET_DIGITAL_WRITE, 1, &data, &data_len))
    {
        data = 0xFF;
    }

    return data == WIFI_SPI_ACK;
}