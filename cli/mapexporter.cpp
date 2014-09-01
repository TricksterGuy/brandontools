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
    try
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
    catch (const std::exception& ex)
    {
        printf("Image to GBA (Map) failed! Reason: %s\n", ex.what());
        exit(EXIT_FAILURE);
    }
    catch (const std::string& ex)
    {
        printf("Image to GBA (Map) failed! Reason: %s\n", ex.c_str());
        exit(EXIT_FAILURE);
    }
    catch (const char* ex)
    {
        printf("Image to GBA (Map) failed! Reason: %s\n", ex);
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        printf("Image to GBA (Map) failed!");
        exit(EXIT_FAILURE);
    }
}
