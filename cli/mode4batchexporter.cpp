#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "shared.hpp"

///TODO Implemented -animated flag in this file.

using Magick::Image;
using namespace std;

static void WriteAll(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params);
static void WriteSeparate(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params);
static void InitFiles(ofstream& file_c, ofstream& file_h, const ExportParams& params);

void DoMode4Multi(std::vector<Magick::Image> images, const ExportParams& params)
{
    /* Ok a bit harder than the mode3 version */
    header.SetMode(4);
    try
    {
        ofstream file_c, file_h;
        InitFiles(file_c, file_h, params);
        std::string name = params.name;
        Chop(name);
        name = Sanitize(name);

        /* The trick is to compose the images and get a global palette */
        for (unsigned int i = 0; i < images.size(); i++)
            images[i] = ConvertToGBA(images[i]);

	    if (!params.split)
            WriteAll(file_c, file_h, images, params);
        else
    		WriteSeparate(file_c, file_h, images, params);

        file_h << "#endif";

        file_c.close();
        file_h.close();
    }
    catch (Magick::Exception &error_ )
    {
        printf("%s\n", error_.what());
        printf("Batch Image to GBA (Mode4) failed!");
        exit(EXIT_FAILURE);
    }
}

static void InitFiles(ofstream& file_c, ofstream& file_h, const ExportParams& params)
{
    std::string name = params.name;
    Chop(name);
    name = Sanitize(name);
    std::string name_cap = name;
    transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

    std::string filename_c = params.name + ".c";
    std::string filename_h = params.name + ".h";

    file_c.open(filename_c.c_str());
    file_h.open(filename_h.c_str());

    if (!file_c.good() || !file_h.good())
    {
        printf("Could not open files for writing\n");
        exit(EXIT_FAILURE);
    }
}

static void WriteAll(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params)
{
    std::string filename_cap = params.name;
    Chop(filename_cap);
    Sanitize(filename_cap);
    transform(filename_cap.begin(), filename_cap.end(), filename_cap.begin(), (int(*)(int)) std::toupper);

    char buffer[7];
    int spacecounter = 0;

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

    std::vector<int>* indexedImages = new std::vector<int>[images.size()];

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
    file_h << "#ifndef " << filename_cap << "_BITMAP_H\n";
    file_h << "#define " << filename_cap << "_BITMAP_H\n\n";

    unsigned int num_colors = palette.size() + params.offset;
    // Error check for p_offset
    if (num_colors > 256)
    {
        printf("Warning: too many colors in palette.\n\
               I don't feel like terminating right now\n\
               But I'd think I'd let you know about this.\n\
               Btw you'd best delete the two files it creates.\n");
    }

    if (params.fullpalette) num_colors = 256;

    file_h << "#define " << filename_cap << "_PALETTE_SIZE " << num_colors << "\n";
    if (params.offset == 0)
        file_h << "\n";
    else
        file_h << "#define " << filename_cap << "_PALETTE_OFFSET " << params.offset << "\n\n";

    file_h << "extern const unsigned short " << params.name << "_palette[" << num_colors << "];\n\n";

    // Write Palette
    file_c << "const unsigned short " << params.name << "_palette" << "[" << num_colors << "] =\n{\n\t";
    for (unsigned int i = 0; i < num_colors; i++)
    {
        unsigned short byte;
        if ((int) i < params.offset)
        {
            // Well I will put dummy colors in here
            byte = 0x8000; // This should be black btw.
        }
        else
        {
            // PACKING
            // pixels[i] = r pixels[i+1] = g pixels[i+2] = b
            // Convert to gba format here
            Color color = (i - params.offset) >= palette.size() ? Color(0, 0, 0) : palette[i - params.offset];
            int r, g, b;
            color.Get(r, g, b);
            byte = r | (g << 5) | (b << 10);
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

    // Write Each Image
    for (unsigned int k = 0; k < images.size(); k++)
    {
        std::string name = params.names[k];

	    if (images[k].columns() % 2)
	    {
                printf("Warning: Image %s width is not a multiple of 2\n\
		    I'm going to continue doing my job since I don't care\n\
		    if you know what you are doing\n\
		    But I'd just thought I'd let you know\n", images[k].comment().c_str());
	    }

        std::string name_cap = name;
        transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

        int num_pixels = images[k].rows() * images[k].columns();
        const std::vector<int>& pixels = indexedImages[k];
        int size = (num_pixels / 2) + ((num_pixels % 2) != 0);

        file_c << "const unsigned short " << name << "[" << size << "] =\n{\n\t";

        spacecounter = 0;
        for (int i = 0; i < size << 1; i += 2)
        {
            int px1 = pixels[i] + params.offset;
            int px2 = pixels[i + 1] + params.offset;
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
        file_c << "\n};\n\n";

        file_h << "extern const unsigned short " << name << "[" << size << "];\n";
        file_h << "#define " << name_cap << "_WIDTH " << images[k].columns() << "\n";
        file_h << "#define " << name_cap << "_HEIGHT " << images[k].rows() << "\n\n";
    }
    delete[] indexedImages;
}

static void WriteSeparate(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params)
{
    char buffer[7];
    int spacecounter = 0;

    std::string filename_cap = params.name;
    Chop(filename_cap);
    filename_cap = Sanitize(filename_cap);
    transform(filename_cap.begin(), filename_cap.end(), filename_cap.begin(), (int(*)(int)) std::toupper);

    header.Write(file_h);
    file_h << "#ifndef " << filename_cap << "_BITMAP_H\n";
    file_h << "#define " << filename_cap << "_BITMAP_H\n\n";

    for (unsigned int k = 0; k < images.size(); k++)
    {
        Image& image = images[k];
        std::string name = params.names[k];
        std::string name_cap = name;
        transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

        unsigned int num_pixels = image.rows() * image.columns();
        int size = (num_pixels / 2) + ((num_pixels % 2) != 0);
        // If the image width is odd warn them
        if (image.columns() % 2 && k == 0)
        {
            printf("Warning: Image width is not a multiple of 2\n\
		    I'm going to continue doing my job since I don't care\n\
		    if you know what you are doing\n\
		    But I'd just thought I'd let you know");
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
            printf("Warning: too many colors in palette.\n\
                   I don't feel like terminating right now\n\
                   But I'd think I'd let you know about this.\n\
                   Btw you'd best delete the two files it creates.\n");
        }
        if (params.fullpalette) num_colors = 256;

        // Write Palette
        if (k == 0)
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


        file_h << "extern const unsigned short " << name << "[" << size << "];\n";
        file_h << "extern const unsigned short " << name << "_palette[" << num_colors << "];\n";
        file_h << "#define " << name_cap << "_WIDTH " << image.columns() << "\n";
        file_h << "#define " << name_cap << "_HEIGHT " << image.rows() << "\n";
        file_h << "#define " << name_cap << "_PALETTE_SIZE " << num_colors << "\n";
        if (params.offset == 0)
            file_h << "\n";
        else
            file_h << "#define " << name_cap << "_PALETTE_OFFSET " << params.offset << "\n\n";
    }

    file_h << "#endif";
    file_c.close();
    file_h.close();
}
