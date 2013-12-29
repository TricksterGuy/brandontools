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
    try
    {
        header.SetMode(0);
        implementation.SetMode(0);

        // Sanity check
        for (const auto& image : images)
        {
            if (image.width % 8 != 0 || image.height % 8 != 0)
            {
                printf("[ERROR] Map image's: %s dimensions must be divisible by 8. Please fix.\n", image.name.c_str());
                exit(EXIT_FAILURE);
            }
            if (image.width > 512 || image.height > 512)
            {
                printf("[ERROR] Map image: %s dimensions must not be greater than 512. Please fix.\n", image.name.c_str());
                exit(EXIT_FAILURE);
            }
            if ((image.width != 32*8 && image.width != 64*8) || (image.height != 32*8 && image.height != 64*8))
            {
                printf("[WARNING] Map image: %s dimensions should be a multiple of 256.\n", image.name.c_str());
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

        // Write the files
        std::ofstream file_c, file_h;
        InitFiles(file_c, file_h, params.name);

        header.Write(file_h);
        implementation.Write(file_c);

        file_h.close();
        file_c.close();
    }
    catch (const std::exception& ex)
    {
        printf("Image to GBA (Mode0 %dbpp) failed! Reason: %s\n", params.bpp, ex.what());
        exit(EXIT_FAILURE);
    }
    catch (const std::string& ex)
    {
        printf("Image to GBA (Mode0 %dbpp) failed! Reason: %s\n", params.bpp, ex.c_str());
        exit(EXIT_FAILURE);
    }
    catch (const char* ex)
    {
        printf("Image to GBA (Mode0 %dbpp) failed! Reason: %s\n", params.bpp, ex);
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        printf("Image to GBA (Mode0 %dbpp) failed!", params.bpp);
        exit(EXIT_FAILURE);
    }
}

