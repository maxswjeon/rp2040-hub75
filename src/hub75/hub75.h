// Heavily based on mrfaptastic/ESP32-HUB75-MatrixPanel-DMA

#ifndef SKKU_BUS_DEFS_H
#define SKKU_BUS_DEFS_H

#include <vector>
#include <memory>
#include <cstdlib>

#define HUB75_T uint16_t

#define BITS_RGB1_OFFSET 0 // Start point of RGB_X1 bits
#define BIT_R1 (1 << 0)
#define BIT_G1 (1 << 1)
#define BIT_B1 (1 << 2)

#define BITS_RGB2_OFFSET 3 // Start point of RGB_X2 bits
#define BIT_R2 (1 << 3)
#define BIT_G2 (1 << 4)
#define BIT_B2 (1 << 5)

#define BIT_LAT (1 << 6)
#define BIT_OE (1 << 7)

#define BITS_ADDR_OFFSET 8 // Start point of RGB_X3 bits
#define BIT_A (1 << 8)
#define BIT_B (1 << 9)
#define BIT_C (1 << 10)
#define BIT_D (1 << 11)
#define BIT_E (1 << 12)

#define BITMASK_RGB1_CLEAR (0b1111111111111000)
#define BITMASK_RGB2_CLEAR (0b1111111111000111)
#define BITMASK_RGBA_CLEAR (0b1111111111000000)
#define BITMASK_CTRL_CLEAR (0b1110000000111111)
#define BITMASK_OE_CLEAR (0b1111111101111111)

#define LAT_BLANKING_DEFAULT 2

#define LAT_BLANKING_MAX 4

namespace HUB75
{
    struct RowBit
    {
        const size_t _width;
        const uint8_t _depth;
        const bool _double_buffer; // sizeof(bool) == sizeof(uint8_t)
        HUB75_T *data;

        RowBit(const size_t width, const uint8_t depth, const bool double_buffer);
        ~RowBit();

        size_t depth_size(const uint8_t depth = 0) const;
        inline HUB75_T *at(const uint8_t depth = 0) const;
    };

    struct Frame
    {
        uint8_t rows = 0;
        std::vector<std::shared_ptr<RowBit>> rowBits;
    };
}

/***************************************************************************************/
//  C/p'ed from https://ledshield.wordpress.com/2012/11/13/led-brightness-to-your-eye-gamma-correction-no/
//  Example calculator: https://gist.github.com/mathiasvr/19ce1d7b6caeab230934080ae1f1380e
//  need to make sure this would end up in RAM for fastest access
#ifndef NO_CIE1931
// This is 16-bit version of the table,
// the constants taken from the example in the article above, each entries subtracted from 65535:
static const uint16_t lumConvTab[] = {
    0, 27, 56, 84, 113, 141, 170, 198, 227, 255, 284, 312, 340, 369, 397, 426,
    454, 483, 511, 540, 568, 597, 626, 657, 688, 720, 754, 788, 824, 860, 898, 936,
    976, 1017, 1059, 1102, 1146, 1191, 1238, 1286, 1335, 1385, 1436, 1489, 1543, 1598, 1655, 1713,
    1772, 1833, 1895, 1958, 2023, 2089, 2156, 2225, 2296, 2368, 2441, 2516, 2592, 2670, 2750, 2831,
    2914, 2998, 3084, 3171, 3260, 3351, 3443, 3537, 3633, 3731, 3830, 3931, 4034, 4138, 4245, 4353,
    4463, 4574, 4688, 4803, 4921, 5040, 5161, 5284, 5409, 5536, 5665, 5796, 5929, 6064, 6201, 6340,
    6482, 6625, 6770, 6917, 7067, 7219, 7372, 7528, 7687, 7847, 8010, 8174, 8341, 8511, 8682, 8856,
    9032, 9211, 9392, 9575, 9761, 9949, 10139, 10332, 10527, 10725, 10925, 11127, 11332, 11540, 11750, 11963,
    12178, 12395, 12616, 12839, 13064, 13292, 13523, 13757, 13993, 14231, 14473, 14717, 14964, 15214, 15466, 15722,
    15980, 16240, 16504, 16771, 17040, 17312, 17587, 17865, 18146, 18430, 18717, 19006, 19299, 19595, 19894, 20195,
    20500, 20808, 21119, 21433, 21750, 22070, 22393, 22720, 23049, 23382, 23718, 24057, 24400, 24745, 25094, 25446,
    25802, 26160, 26522, 26888, 27256, 27628, 28004, 28382, 28765, 29150, 29539, 29932, 30328, 30727, 31130, 31536,
    31946, 32360, 32777, 33197, 33622, 34049, 34481, 34916, 35354, 35797, 36243, 36692, 37146, 37603, 38064, 38528,
    38996, 39469, 39945, 40424, 40908, 41395, 41886, 42382, 42881, 43383, 43890, 44401, 44916, 45434, 45957, 46484,
    47014, 47549, 48088, 48630, 49177, 49728, 50283, 50842, 51406, 51973, 52545, 53120, 53700, 54284, 54873, 55465,
    56062, 56663, 57269, 57878, 58492, 59111, 59733, 60360, 60992, 61627, 62268, 62912, 63561, 64215, 64873, 65535};
#endif

#endif // SKKU_BUS_DEFS_H
