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

void DoMode4(const std::vector<Image16Bpp>& images)
{
    // Set Mode
    header.SetMode(4);
    implementation.SetMode(4);

    // Do the work of mode 4 conversion.
    // If split then get vector of 8 bit images
    // If !split then cause a scene.
    // Add appropriate object to header/implementation.
    if (params.split)
    {
        bool first = true;
        for (const auto& image : images)
        {
            std::shared_ptr<Image8Bpp> image_ptr(new Image8Bpp(image));
            header.AddImage(image_ptr);
            implementation.AddImage(image_ptr);
            if (first)
            {
                header.SetPalette(image_ptr->palette);
                implementation.SetPalette(image_ptr->palette);
            }
        }
    }
    else
    {
        std::shared_ptr<Image8BppScene> scene(new Image8BppScene(images, params.symbol_base_name));
        header.SetPalette(scene->palette);
        implementation.SetPalette(scene->palette);
        header.AddScene(scene);
        implementation.AddScene(scene);
    }
}
