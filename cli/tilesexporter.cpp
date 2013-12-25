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
    try
    {
        // Set mode
        header.SetMode(0);
        implementation.SetMode(0);

        // Do the work of mode 0 conversion.
        // Form the tileset and then add it to header and implementation
        std::shared_ptr<Tileset> tileset(new Tileset(images, params.name, params.bpp));

        header.AddTileset(tileset);
        implementation.AddTileset(tileset);

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
        printf("Image to GBA (tiles) failed! Reason: %s\n", ex.what());
        exit(EXIT_FAILURE);
    }
    catch (const std::string& ex)
    {
        printf("Image to GBA (tiles) failed! Reason: %s\n", ex.c_str());
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        printf("Image to GBA (tiles) failed!");
        exit(EXIT_FAILURE);
    }
}

