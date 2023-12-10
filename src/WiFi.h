#ifndef _SKKU_BUS_WIFI_H
#define _SKKU_BUS_WIFI_H

// #define SPI_CHANNEL spi1
// #define SPI_CSn_PIN 9
// #define SPI_ACK_PIN 10
// #define SPI_SCK_PIN 14
// #define SPI_MISO_PIN 8
// #define SPI_MOSI_PIN 11

#define SPI_CHANNEL spi1
#define SPI_CSn_PIN 9
#define SPI_ACK_PIN 10
#define SPI_SCK_PIN 14
#define SPI_MISO_PIN 8
#define SPI_MOSI_PIN 11
#define SPI_RESET_PIN 24

#define LEDR 27
#define LEDG 25
#define LEDB 26
#define A4 34
#define A5 39
#define A6 36
#define A7 35

#include "pico/stdlib.h"
#include "hardware/spi.h"

enum
{
    SET_NET_CMD = 0x10,
    SET_PASSPHRASE_CMD = 0x11,
    SET_KEY_CMD = 0x12,
    //	TEST_CMD	        = 0x13,
    SET_IP_CONFIG_CMD = 0x14,
    SET_DNS_CONFIG_CMD = 0x15,
    SET_HOSTNAME_CMD = 0x16,
    SET_POWER_MODE_CMD = 0x17,
    SET_AP_NET_CMD = 0x18,
    SET_AP_PASSPHRASE_CMD = 0x19,
    SET_DEBUG_CMD = 0x1A,
    GET_TEMPERATURE_CMD = 0x1B,
    GET_REASON_CODE_CMD = 0x1F,

    GET_CONN_STATUS_CMD = 0x20,
    GET_IPADDR_CMD = 0x21,
    GET_MACADDR_CMD = 0x22,
    GET_CURR_SSID_CMD = 0x23,
    GET_CURR_BSSID_CMD = 0x24,
    GET_CURR_RSSI_CMD = 0x25,
    GET_CURR_ENCT_CMD = 0x26,
    SCAN_NETWORKS = 0x27,
    START_SERVER_TCP_CMD = 0x28,
    GET_STATE_TCP_CMD = 0x29,
    DATA_SENT_TCP_CMD = 0x2A,
    AVAIL_DATA_TCP_CMD = 0x2B,
    GET_DATA_TCP_CMD = 0x2C,
    START_CLIENT_TCP_CMD = 0x2D,
    STOP_CLIENT_TCP_CMD = 0x2E,
    GET_CLIENT_STATE_TCP_CMD = 0x2F,
    DISCONNECT_CMD = 0x30,
    //	GET_IDX_SSID_CMD	= 0x31,
    GET_IDX_RSSI_CMD = 0x32,
    GET_IDX_ENCT_CMD = 0x33,
    REQ_HOST_BY_NAME_CMD = 0x34,
    GET_HOST_BY_NAME_CMD = 0x35,
    START_SCAN_NETWORKS = 0x36,
    GET_FW_VERSION_CMD = 0x37,
    //	GET_TEST_CMD		= 0x38,
    SEND_DATA_UDP_CMD = 0x39,
    GET_REMOTE_DATA_CMD = 0x3A,
    GET_TIME_CMD = 0x3B,
    GET_IDX_BSSID = 0x3C,
    GET_IDX_CHANNEL_CMD = 0x3D,
    PING_CMD = 0x3E,
    GET_SOCKET_CMD = 0x3F,

    // All command with DATA_FLAG 0x40 send a 16bit Len
    SET_ENT_CMD = 0x40,

    SEND_DATA_TCP_CMD = 0x44,
    GET_DATABUF_TCP_CMD = 0x45,
    INSERT_DATABUF_CMD = 0x46,

    // regular format commands
    SET_PIN_MODE = 0x50,
    SET_DIGITAL_WRITE = 0x51,
    SET_ANALOG_WRITE = 0x52,
    GET_DIGITAL_READ = 0x53,
    GET_ANALOG_READ = 0x54,

    // regular format commands
    WRITE_FILE = 0x60,
    READ_FILE = 0x61,
    DELETE_FILE = 0x62,
    EXISTS_FILE = 0x63,
    DOWNLOAD_FILE = 0x64,
    APPLY_OTA_COMMAND = 0x65,
    RENAME_FILE = 0x66,
    DOWNLOAD_OTA = 0x67,
};

enum wl_tcp_state
{
    CLOSED = 0,
    LISTEN = 1,
    SYN_SENT = 2,
    SYN_RCVD = 3,
    ESTABLISHED = 4,
    FIN_WAIT_1 = 5,
    FIN_WAIT_2 = 6,
    CLOSE_WAIT = 7,
    CLOSING = 8,
    LAST_ACK = 9,
    TIME_WAIT = 10
};

enum numParams
{
    PARAM_NUMS_0,
    PARAM_NUMS_1,
    PARAM_NUMS_2,
    PARAM_NUMS_3,
    PARAM_NUMS_4,
    PARAM_NUMS_5,
    PARAM_NUMS_6,
    MAX_PARAM_NUMS
};

typedef enum
{
    INPUT = 0x0,
    OUTPUT = 0x1,
    INPUT_PULLUP = 0x2,
    INPUT_PULLDOWN = 0x3,
    OUTPUT_OPENDRAIN = 0x4,
} PinMode;

void wifi_init();
bool wifi_set_ap(const char *ssid, uint8_t ssid_len, uint8_t channel);
bool wifi_set_ap(const char *ssid, uint8_t ssid_len, const char *psk, uint8_t psk_len, uint8_t channel);
bool wifi_gpio_init(uint8_t pin);
bool wifi_gpio_set_dir(uint8_t pin, uint8_t dir);
bool wifi_gpio_put(uint8_t pin, uint8_t val);

#endif