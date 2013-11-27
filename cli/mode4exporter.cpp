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
        printf("Image to GBA (Mode4) failed!");
        exit(EXIT_FAILURE);
    }
}

static void WriteC(Image image, const ExportParams& params)
{
    ofstream file_c, file_h;

    std::string filename_c = params.name + ".c";
    std::string filename_h = params.name + ".h";
    std::string name = params.name;
    Chop(name);
    name = Sanitize(name);
    std::string name_cap = name;
    char buffer[7];
    int spacecounter = 0;

    transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

    file_c.open(filename_c.c_str());
    file_h.open(filename_h.c_str());

    if (!file_c.good() || !file_h.good())
    {
        printf("Could not open files for writing\n");
        exit(EXIT_FAILURE);
    }

    unsigned int num_pixels = image.rows() * image.columns();
    int size = (num_pixels / 2) + ((num_pixels % 2) != 0);
    // If the image width is odd warn them
    if (image.columns() % 2)
    {
        printf("[WARNING] Image width is not a multiple of 2\n\
		I'm going to continue doing my job since I don't care\n\
		if you know what you are doing\n\
		But I'd just thought I'd let you know\n");
    }

    unsigned char* imagePixels = new unsigned char[num_pixels * 3];
    std::vector<Color> pixels(num_pixels);
    palette.clear();
    image.write(0, 0, image.columns(), image.rows(), "RGB", Magick::CharPixel, imagePixels);

    for (unsigned int i = 0; i < num_pixels; i++)
        pixels[i].Set(imagePixels[3 * i + 0], imagePixels[3 * i + 1], imagePixels[3 * i + 2]);
    delete[] imagePixels;

    MedianCut(pixels, params.palette, palette, params.weights);
    std::vector<int> indexedImage(num_pixels, 1);
    RiemersmaDither(pixels.begin(), indexedImage, image.columns(), image.rows(), params.dither, params.dither_level);

    unsigned int num_colors = params.offset + palette.size();
    // Error check for p_offset
    if (num_colors > 256)
    {
        printf("[WARNING] too many colors in palette.\n\
               I don't feel like terminating right now\n\
               But I'd think I'd let you know about this.\n\
               Btw you'd best delete the two files it creates.\n");
    }
    if (params.fullpalette) num_colors = 256;

    // Write Palette
    header.Write(file_c);
    file_c << "const unsigned short " << name << "_palette" << "[" << num_colors << "] =\n{\n\t";
    for (unsigned int i = 0; i < num_colors; i++)
    {

        unsigned short byte;
        if ((int) i < params.offset)
        {
            // Well I will put dummy colors in here
            byte = 0x8000; // This is black btw.
        }
        else
        {
            // PACKING
            // pixels[i] = r pixels[i+1] = g pixels[i+2] = b
            // Convert to gba format here
            Color color = (i - params.offset) >= palette.size() ? Color(0, 0, 0) : palette[i - params.offset];
            int x, y, z;
            color.Get(x, y, z);
            byte = x | (y << 5) | (z << 10);
        }
        sprintf(buffer, "0x%04x", byte);
        file_c << buffer;
        if (i != num_colors - 1)
        {
            file_c << ",";
            spacecounter++;
            if (spacecounter == 10)
            {
                file_c << "\n\t";
                spacecounter = 0;
            }
        }
    }
    file_c << "\n};\n\n";

    // Write Palette Entries
    file_c << "const unsigned short " << name << "[" << size << "] =\n{\n\t";
    spacecounter = 0;
    for (int i = 0; i < size << 1; i += 2)
    {
        // Increase by p_offset here
        int px1 = indexedImage[i] + params.offset;
        int px2 = indexedImage[i + 1] + params.offset;
        //assert(px1 < palette.size());
        //assert(px2 < palette.size());
        unsigned short byte = px1 | (px2 << 8);
        sprintf(buffer, "0x%04x", byte);
        file_c << buffer;
        if (i != (size - 1) << 1)
        {
            file_c << ",";
            spacecounter++;
            if (spacecounter == 10)
            {
                file_c << "\n\t";
                spacecounter = 0;
            }
        }
    }
    file_c << "\n};\n";

    header.Write(file_h);
    file_h << "#ifndef " << name_cap << "_BITMAP_H\n";
    file_h << "#define " << name_cap << "_BITMAP_H\n\n";
    file_h << "extern const unsigned short " << name << "[" << size << "];\n";
    file_h << "extern const unsigned short " << name << "_palette[" << num_colors << "];\n";
    file_h << "#define " << name_cap << "_WIDTH " << image.columns() << "\n";
    file_h << "#define " << name_cap << "_HEIGHT " << image.rows() << "\n";
    file_h << "#define " << name_cap << "_PALETTE_SIZE " << num_colors << "\n";
    if (params.offset == 0)
        file_h << "\n";
    else
        file_h << "#define " << name_cap << "_PALETTE_OFFSET " << params.offset << "\n\n";
    file_h << "#endif";

    file_c.close();
    file_h.close();
}
