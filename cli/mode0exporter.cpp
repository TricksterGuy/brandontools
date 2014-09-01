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

void DoMode0(const std::vector<Image16Bpp>& images)
{
    header.SetMode(0);
    implementation.SetMode(0);

    // Sanity check
    for (const auto& image : images)
    {
        if (image.width % 8 != 0 || image.height % 8 != 0)
        {
            fprintf(stderr, "[ERROR] Map image's: %s dimensions must be divisible by 8. Please fix.\n", image.name.c_str());
            exit(EXIT_FAILURE);
        }
        if (image.width > 512 || image.height > 512)
        {
            fprintf(stderr, "[ERROR] Map image: %s dimensions must not be greater than 512. Please fix.\n", image.name.c_str());
            exit(EXIT_FAILURE);
        }
        if ((image.width != 32*8 && image.width != 64*8) || (image.height != 32*8 && image.height != 64*8))
        {
            fprintf(stderr, "[WARNING] Map image: %s dimensions should be a multiple of 256.\n", image.name.c_str());
        }
    }

    // Do the work of mode 0 conversion
    // If split then form several maps
    // If !split then start a scene
    // Add appropriate object to header/implementation
    if (params.split)
    {
        bool first = true;
        for (const auto& image : images)
        {
            std::shared_ptr<Map> map_ptr(new Map(image, params.bpp));
            header.AddMapSet(map_ptr);
            implementation.AddMapSet(map_ptr);
            if (first)
            {
                header.SetPalette(map_ptr->tileset->palette);
                implementation.SetPalette(map_ptr->tileset->palette);
            }
        }
    }
    else
    {
        std::shared_ptr<MapScene> scene(new MapScene(images, params.name, params.bpp));
        header.AddScene(scene);
        implementation.AddScene(scene);
        header.SetPalette(scene->tileset->palette);
        implementation.SetPalette(scene->tileset->palette);
    }
}

