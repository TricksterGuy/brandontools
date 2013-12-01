#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include "shared.hpp"
#include "tile.hpp"

using namespace Magick;
using namespace std;

static void WriteC(Image image, const ExportParams& params);

void DoMode0_4bpp(Magick::Image image, const ExportParams& params)
{
}
