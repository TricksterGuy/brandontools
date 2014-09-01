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
    try
    {
        // Set mode
        header.SetMode(0);
        implementation.SetMode(0);

        // Do the work of sprite conversion.
        // Form the sprite scene and then add it to header and implementation
        std::shared_ptr<SpriteScene> scene(new SpriteScene(images, params.name, params.export_2d, params.bpp));

        // Build the sprite scene and place all sprites. (If applicable)
        scene->Build();

        header.SetPalette(scene->palette);
        implementation.SetPalette(scene->palette);

        header.AddScene(scene);
        implementation.AddScene(scene);
    }
    catch (const std::exception& ex)
    {
        printf("Image to GBA (sprites) failed! Reason: %s\n", ex.what());
        exit(EXIT_FAILURE);
    }
    catch (const std::string& ex)
    {
        printf("Image to GBA (sprites) failed! Reason: %s\n", ex.c_str());
        exit(EXIT_FAILURE);
    }
    catch (const char* ex)
    {
        printf("Image to GBA (sprites) failed! Reason: %s\n", ex);
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        printf("Image to GBA (sprites) failed!");
        exit(EXIT_FAILURE);
    }
}
