#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include "shared.hpp"
#include "tile.hpp"

using namespace Magick;
using namespace std;

void DoMode0_4bpp(Magick::Image image, const ExportParams& params)
{
    header.SetMode(0);
    try
    {

    }
    catch (Magick::Exception &error_)
    {
        printf("%s\n", error_.what());
        printf("[ERROR] Image to GBA (Mode0 4bpp) failed!");
        exit(EXIT_FAILURE);
    }
}
