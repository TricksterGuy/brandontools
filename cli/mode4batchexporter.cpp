#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include "shared.hpp"
#include "reductionhelper.hpp"

using namespace Magick;
using namespace std;

static void WriteC(std::vector<Magick::Image> images, const ExportParams& params);
static void WriteAll(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params);
static void WriteSeparate(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params);

void DoMode4Multi(std::vector<Magick::Image> images, const ExportParams& params)
{
    /* Ok a bit harder than the mode3 version */
    header.SetMode(4);
    try
    {
        WriteC(images, params);
    }
    catch (Magick::Exception &error_ )
    {
        printf("%s\n", error_.what());
        printf("[ERROR] Batch Image to GBA (Mode4) failed!");
        exit(EXIT_FAILURE);
    }
}

void WriteC(std::vector<Magick::Image> images, const ExportParams& params)
{
    ofstream file_c, file_h;
    InitFiles(file_c, file_h, params.name);
    std::string name = Format(params.name);

    /* The trick is to compose the images and get a global palette */

    // -split allows you to generate multiple palettes for each image given
    // useful for videos.
    if (params.split)
        WriteSeparate(file_c, file_h, images, params);
    else
        WriteAll(file_c, file_h, images, params);


    WriteEndHeaderGuard(file_h);
    WriteNewLine(file_h);
    file_h.close();

    WriteNewLine(file_c);
    file_c.close();
}

void WriteAll(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params)
{
    std::string filename_cap = ToUpper(Format(params.name));

    std::vector<IndexedImage> indexedImages(images.size());
    FormPaletteAndIndexedImages(images, params, indexedImages);

    // Error check for p_offset
    unsigned int num_colors = palette.size() + params.offset;
    if (num_colors > 256)
    {
        printf("[WARNING] too many colors in palette.\n\
               I don't feel like terminating right now\n\
               But I'd think I'd let you know about this.\n\
               Btw you'd best delete the two files it creates.\n");
    }
    if (params.fullpalette) num_colors = 256;

    // This has to happen after you form the palette
    header.Write(file_h);
    WriteHeaderGuard(file_h, filename_cap, "_BITMAP_H");
    WriteExternShortArray(file_h, params.name, "_palette", num_colors);
    WriteDefine(file_h, filename_cap, "_PALETTE_OFFSET", params.offset);
    WriteDefine(file_h, filename_cap, "_PALETTE_SIZE", num_colors);
    WriteNewLine(file_h);

    header.Write(file_c);
    WriteShortArray(file_c, params.name, "_palette", palette.data(), num_colors, GetPaletteEntry, 10, &params.offset);
    WriteNewLine(file_c);

    // Handle -animated
    if (params.animated)
    {
        WriteExternShortPtrArray(file_h, params.name, "_frames", images.size());
        WriteDefine(file_h, filename_cap, "_ANIMATION_FRAMES", images.size());
        WriteNewLine(file_h);
        WriteNewLine(file_h);

        WriteShortPtrArray(file_c, params.name, "_frames", params.names.data(), images.size(), getAnimFrameName, 1);
        WriteNewLine(file_c);
    }

    // Write Each Image
    for (unsigned int k = 0; k < images.size(); k++)
    {
        std::string name = params.names[k];
        std::string name_cap = ToUpper(name);

        if (images[k].columns() % 2)
            printf("[WARNING] Image %s width is not a multiple of 2\n", name.c_str());

        int num_pixels = images[k].rows() * images[k].columns();
        int size = (num_pixels / 2) + ((num_pixels % 2) != 0);
        const std::vector<unsigned char>& pixels = indexedImages[k];

        WriteShortArray(file_c, name, "", pixels.data(), size, GetIndexedEntry, 10, &params.offset);
        WriteNewLine(file_c);

        WriteExternShortArray(file_h, name, "", size);
        WriteDefine(file_h, name_cap, "_WIDTH", images[k].columns());
        WriteDefine(file_h, name_cap, "_HEIGHT", images[k].rows());
        WriteNewLine(file_h);
    }
}

void WriteSeparate(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params)
{
    std::string filename_cap = ToUpper(Format(params.name));

    for (unsigned int k = 0; k < images.size(); k++)
    {
        palette.clear();
        std::string name = params.names[k];
        std::string name_cap = ToUpper(name);

        Image& image = images[k];
        unsigned int num_pixels = image.rows() * image.columns();
        int size = (num_pixels / 2) + ((num_pixels % 2) != 0);
        IndexedImage indexedImage(num_pixels, 1);

        QuantizeImage(image, params, indexedImage);
        unsigned int num_colors = params.offset + palette.size();
        // Error check for p_offset
        if (num_colors > 256)
        {
            printf("[ERROR] too many colors in palette.\n\
                    Found %d colors, offset is %d", num_colors,
                   params.offset);
            exit(EXIT_FAILURE);
        }
        if (params.fullpalette) num_colors = 256;

        // Write Headers.  This can only be done after the palette is generated
        if (k == 0)
        {
            header.Write(file_c);

            header.Write(file_h);
            WriteHeaderGuard(file_h, filename_cap, "_BITMAP_H");
            WriteDefine(file_h, filename_cap, "_PALETTE_OFFSET", params.offset);
            WriteNewLine(file_h);

            // Handle -animated
            if (params.animated)
            {
                WriteExternShortPtrArray(file_h, name, "_frames", images.size());
                WriteDefine(file_h, name_cap, "_ANIMATION_FRAMES", images.size());
                WriteNewLine(file_h);
                WriteNewLine(file_h);

                WriteShortPtrArray(file_c, name, "_frames", params.names.data(), images.size(), getAnimFrameName, 1);
                WriteNewLine(file_c);
            }
        }

        // Write palette
        WriteShortArray(file_c, name, "_palette", palette.data(), num_colors, GetPaletteEntry, 10, &params.offset);
        WriteNewLine(file_c);
        WriteShortArray(file_c, name, "", indexedImage.data(), size, GetIndexedEntry, 10, &params.offset);
        WriteNewLine(file_c);

        WriteExternShortArray(file_h, name, "_palette", num_colors);
        WriteDefine(file_h, name_cap, "_PALETTE_SIZE", num_colors);
        WriteExternShortArray(file_h, name, "", size);
        WriteDefine(file_h, name_cap, "_WIDTH", image.columns());
        WriteDefine(file_h, name_cap, "_HEIGHT", image.rows());
    }
}
