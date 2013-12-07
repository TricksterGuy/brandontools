#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "shared.hpp"
#include <cassert>

using Magick::Image;
using namespace std;

static void WriteC(Image image, const ExportParams& params);

void DoMode4(Image image, const ExportParams& params)
{
    header.SetMode(4);
    try
    {
        image = ConvertToGBA(image);
        WriteC(image, params);
    }
    catch (Magick::Exception &error_)
    {
        printf("%s\n", error_.what());
        printf("[ERROR] Image to GBA (Mode4) failed!");
        exit(EXIT_FAILURE);
    }
}

static void WriteC(Image image, const ExportParams& params)
{
    ofstream file_c, file_h;
    InitFiles(file_c, file_h, params.name);
    std::string name = Format(params.name);
    std::string name_cap = name;
    transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

    unsigned int num_pixels = image.rows() * image.columns();
    unsigned int size = (num_pixels / 2) + ((num_pixels % 2) != 0);
    std::vector<unsigned char> indexedImage(num_pixels, 1);
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

    // Sanity check image width and warn
    if (image.columns() % 2)
    {
        printf("[WARNING] Image width is not a multiple of 2\n");
    }

    // Write Headers
    header.Write(file_c);
    header.Write(file_h);

    // Write Palette and Image Data
    WriteShortArray(file_c, name, "_palette", palette.data(), num_colors, GetPaletteEntry, 10, &params.offset);
    WriteNewLine(file_c);
    WriteShortArray(file_c, name, "", indexedImage.data(), size, GetIndexedEntry, 10, &params.offset);
    WriteNewLine(file_c);
    file_c.close();

    // Write Header file
    WriteHeaderGuard(file_h, name_cap, "_BITMAP_H");
    WriteExternShortArray(file_h, name, "_palette", num_colors);
    WriteExternShortArray(file_h, name, "", size);
    WriteNewLine(file_h);
    WriteDefine(file_h, name_cap, "_WIDTH", image.columns());
    WriteDefine(file_h, name_cap, "_HEIGHT", image.rows());
    if (params.offset)
        WriteDefine(file_h, name_cap, "_PALETTE_OFFSET", params.offset);
    WriteDefine(file_h, name_cap, "_PALETTE_SIZE", num_colors);
    WriteNewLine(file_h);
    WriteEndHeaderGuard(file_h);
    file_h.close();
}
