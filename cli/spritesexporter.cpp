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

void DoSpriteExport(const std::vector<Image16Bpp>& images)
{
    // Set mode
    header.SetMode(0);
    implementation.SetMode(0);

    std::shared_ptr<SpriteScene> scene;

    // Special case if someone uses a spritesheet
    if (images.size() == 1 && (images[0].width > 64 || images[0].height > 64))
    {
        // Nastygram
        bool using2d = !(images[0].width == 8 || images[0].height == 8);
        std::cerr << "[WARNING] Spritesheet detected.\n";
        std::cerr << "If you formed your sprites in a single sprite sheet please note that \n";
        std::cerr << "the program will automatically build the spritesheet for you and export.\n";
        std::cerr << "Just pass in the images you want to use as sprites and let me do the rest.\n";
        std::cerr << "Override: using sprite mode " << (using2d ? "2D" : "1D") << "\n";
        scene.reset(new SpriteScene(images[0], params.symbol_base_name, true, params.bpp));
    }
    else
    {
        // Do the work of sprite conversion.
        // Form the sprite scene and then add it to header and implementation
        scene.reset(new SpriteScene(images, params.symbol_base_name, params.export_2d, params.bpp));
    }

    // Build the sprite scene and place all sprites. (If applicable)
    scene->Build();

    header.SetPalette(scene->palette);
    implementation.SetPalette(scene->palette);

    header.AddScene(scene);
    implementation.AddScene(scene);
}
