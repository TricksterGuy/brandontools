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

void DoMode3(const std::vector<Image16Bpp>& images)
{
    try
    {
        // Set mode
        header.SetMode(3);
        implementation.SetMode(3);

        // Add images to header and implemenation files
        for (const auto& image : images)
        {
            std::shared_ptr<Image16Bpp> image_ptr(new Image16Bpp(image));
            header.AddImage(image_ptr);
            implementation.AddImage(image_ptr);
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
        printf("Image to GBA (Mode3) failed! Reason: %s\n", ex.what());
        exit(EXIT_FAILURE);
    }
    catch (const std::string& ex)
    {
        printf("Image to GBA (Mode3) failed! Reason: %s\n", ex.c_str());
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        printf("Image to GBA (Mode3) failed!");
        exit(EXIT_FAILURE);
    }
}
