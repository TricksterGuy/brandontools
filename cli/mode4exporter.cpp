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
    try
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
            std::shared_ptr<Image8BppScene> scene(new Image8BppScene(images, params.name));
            header.SetPalette(scene->palette);
            implementation.SetPalette(scene->palette);
            header.AddScene(scene);
            implementation.AddScene(scene);
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
        printf("Image to GBA (Mode4) failed! Reason: %s\n", ex.what());
        exit(EXIT_FAILURE);
    }
    catch (const std::string& ex)
    {
        printf("Image to GBA (Mode4) failed! Reason: %s\n", ex.c_str());
        exit(EXIT_FAILURE);
    }
    catch (...)
    {
        printf("Image to GBA (Mode4) failed!");
        exit(EXIT_FAILURE);
    }
}
