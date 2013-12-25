#ifndef DITHER_HPP
#define DITHER_HPP

#include <iterator>
#include <memory>
#include <vector>

#include "color.hpp"
#include "reductionhelper.hpp"

void RiemersmaDither(std::vector<Color>::iterator pixels, Image8Bpp& image, int dither, float ditherlevel);

#endif
