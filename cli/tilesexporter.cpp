#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "fileutils.hpp"
#include "reductionhelper.hpp"
#include "shared.hpp"

void DoTilesetExport(const std::vector<Image16Bpp>& images)
{
    // Set mode
    header.SetMode(0);
    implementation.SetMode(0);

    // Do the work of mode 0 conversion.
    // Form the tileset and then add it to header and implementation
    std::shared_ptr<Tileset> tileset(new Tileset(images, params.name, params.bpp));

    header.SetPalette(tileset->palette);
    implementation.SetPalette(tileset->palette);

    header.AddTileset(tileset);
    implementation.AddTileset(tileset);
}

