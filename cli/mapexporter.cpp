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

void DoMapExport(const std::vector<Image16Bpp>& images, const std::vector<Image16Bpp>& tilesets)
{
    header.SetMode(0);
    implementation.SetMode(0);

    // Form the tileset from the images given this is a dummy
    std::shared_ptr<Tileset> tileset(new Tileset(tilesets, "", params.bpp));

    header.SetPalette(tileset->palette);
    implementation.SetPalette(tileset->palette);

    for (const auto& image : images)
    {
        std::shared_ptr<Map> map_ptr(new Map(image, tileset));
        header.AddMap(map_ptr);
        implementation.AddMap(map_ptr);
    }
}
