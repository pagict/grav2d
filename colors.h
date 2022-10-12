#ifndef _GRAV2D_COLORS_H_
#define _GRAV2D_COLORS_H_
#include <tuple>

using RGBi = std::tuple<int, int, int>;
using RGBf = std::tuple<float, float, float>;

using RGBAuc =
    std::tuple<unsigned char, unsigned char, unsigned char, unsigned char>;
using RGBAf = std::tuple<float, float, float, float>;

static const RGBAf kRGBfWhite = {1.0, 1.0, 1.0, 1.0};

#endif // !_GRAV2D_COLORS_H_