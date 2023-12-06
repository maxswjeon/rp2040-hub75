#include "hub75.h"

using namespace HUB75;

RowBit::RowBit(const size_t width, const uint8_t depth, const bool double_buffer)
    : _width(width), _depth(depth), _double_buffer(double_buffer)
{
    data = (HUB75_T *)malloc(depth_size());
}

RowBit::~RowBit()
{
    free(this->data);
}

size_t RowBit::depth_size(const uint8_t depth) const
{
    return depth ? _width * depth * sizeof(HUB75_T) : _width * _depth * sizeof(HUB75_T);
}

inline HUB75_T *RowBit::at(const uint8_t depth = 0) const
{
    return data + depth * _width;
}
