#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include "shared.hpp"

using Magick::Image;
using namespace std;

static void WriteAll(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params);
static void WriteSeparate(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params);

void DoMode4Multi(std::vector<Magick::Image> images, const ExportParams& params)
{
    /* Ok a bit harder than the mode3 version */
    header.SetMode(4);
    try
    {
        ofstream file_c, file_h;
        InitFiles(file_c, file_h, params.name);
        std::string name = Format(params.name);

        /* The trick is to compose the images and get a global palette */
        for (unsigned int i = 0; i < images.size(); i++)
            images[i] = ConvertToGBA(images[i]);

        // -split allows you to generate multiple palettes for each image given
        // useful for videos.
	    if (params.split)
    		WriteSeparate(file_c, file_h, images, params);
        else
            WriteAll(file_c, file_h, images, params);

        WriteEndHeaderGuard(file_h);

        file_c.close();
        file_h.close();
    }
    catch (Magick::Exception &error_ )
    {
        printf("%s\n", error_.what());
        printf("[ERROR] Batch Image to GBA (Mode4) failed!");
        exit(EXIT_FAILURE);
    }
}

static void WriteAll(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params)
{
    std::string filename_cap = Format(params.name);
    transform(filename_cap.begin(), filename_cap.end(), filename_cap.begin(), (int(*)(int)) std::toupper);

    // TODO make this more memory efficient.
    unsigned int total_pixels = 0;
    for (unsigned int i = 0; i < images.size(); i++)
        total_pixels += images[i].rows() * images[i].columns();
    unsigned char* all_pixels = new unsigned char[total_pixels * 3];
    unsigned char* current = all_pixels;

    for (unsigned int i = 0; i < images.size(); i++)
    {
        images[i].write(0, 0, images[i].columns(), images[i].rows(), "RGB", Magick::CharPixel, current);
        current += images[i].columns() * images[i].rows() * 3;
    }
    std::vector<Color> pixels(total_pixels);
    for (unsigned int i = 0; i < total_pixels; i++)
        pixels[i].Set(all_pixels[3 * i + 0], all_pixels[3 * i + 1], all_pixels[3 * i + 2]);
    delete[] all_pixels;
    MedianCut(pixels, params.palette, palette, params.weights);

    std::vector<unsigned char>* indexedImages = new std::vector<unsigned char>[images.size()];

    // Dither and form all images
    int location = 0;
    for (unsigned int i = 0; i < images.size(); i++)
    {
        indexedImages[i].resize(images[i].rows() * images[i].columns(), 1);
        RiemersmaDither(pixels.begin() + location, indexedImages[i], images[i].columns(), images[i].rows(), params.dither, params.dither_level);
        location += images[i].columns() * images[i].rows();
    }

    // This has to happen after you form the palette
    header.Write(file_c);
    header.Write(file_h);
    WriteHeaderGuard(file_h, filename_cap, "_BITMAP_H");

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

    WriteExternShortArray(file_h, params.name, "_palette", num_colors);
    WriteDefine(file_h, filename_cap, "_PALETTE_OFFSET", params.offset);
    WriteDefine(file_h, filename_cap, "_PALETTE_SIZE", num_colors);
    WriteNewLine(file_h);

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
        std::string name_cap = name;
        transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

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
    delete[] indexedImages;
}

static void WriteSeparate(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params)
{
    std::string filename_cap = Format(params.name);
    transform(filename_cap.begin(), filename_cap.end(), filename_cap.begin(), (int(*)(int)) std::toupper);

    for (unsigned int k = 0; k < images.size(); k++)
    {
        palette.clear();

        Image& image = images[k];
        std::string name = params.names[k];
        std::string name_cap = name;
        transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

        unsigned int num_pixels = image.rows() * image.columns();
        int size = (num_pixels / 2) + ((num_pixels % 2) != 0);
        // If the image width is odd warn them
        if (image.columns() % 2 && k == 0)
            printf("[WARNING] Image width is not a multiple of 2\n");

        unsigned char* imagePixels = new unsigned char[num_pixels * 3];
        image.write(0, 0, image.columns(), image.rows(), "RGB", Magick::CharPixel, imagePixels);

        std::vector<Color> pixels(num_pixels);
        for (unsigned int i = 0; i < num_pixels; i++)
            pixels[i].Set(imagePixels[3 * i + 0], imagePixels[3 * i + 1], imagePixels[3 * i + 2]);
        delete[] imagePixels;
        MedianCut(pixels, params.palette, palette, params.weights);

        std::vector<unsigned char> indexedImage(num_pixels, 1);
        RiemersmaDither(pixels.begin(), indexedImage, image.columns(), image.rows(), params.dither, params.dither_level);

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

        // Write Palette Entries
        WriteShortArray(file_c, name, "", indexedImage.data(), size, GetIndexedEntry, 10, &params.offset);
        WriteNewLine(file_c);


        WriteExternShortArray(file_h, name, "_palette", num_colors);
        WriteDefine(file_h, name_cap, "_PALETTE_SIZE", num_colors);
        WriteExternShortArray(file_h, name, "", size);
        WriteDefine(file_h, name_cap, "_WIDTH", image.columns());
        WriteDefine(file_h, name_cap, "_HEIGHT", image.rows());
    }

    WriteEndHeaderGuard(file_h);

    file_h.close();
    file_c.close();
}
