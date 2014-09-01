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
    // Set mode
    header.SetMode(3);
    implementation.SetMode(3);

    // Add images to header and implementation files
    for (const auto& image : images)
    {
        std::shared_ptr<Image16Bpp> image_ptr(new Image16Bpp(image));
        header.AddImage(image_ptr);
        implementation.AddImage(image_ptr);
    }
}
