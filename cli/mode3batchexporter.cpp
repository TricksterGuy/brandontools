#include <Magick++.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "shared.hpp"

using namespace Magick;
using namespace std;

static void WriteOut(ofstream& file_c, ofstream& file_h, Magick::Image image, const ExportParams& params);
static void InitFiles(ofstream& file_c, ofstream& file_h, const ExportParams& params);

void DoMode3Multi(std::vector<Magick::Image> images, const ExportParams& params)
{
    header.SetMode(3);
    try
    {
        std::string name_cap = params.name;
        transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);
        ofstream file_c, file_h;
        InitFiles(file_c, file_h, params);

        if (params.transparent_color != -1)
        {
            file_h << "#define " << name_cap << "_TRANSPARENT 0x" << std::hex << (unsigned short) params.transparent_color << std::dec << "\n\n";
        }

        if (params.animated)
        {
            file_h << "#define " << name_cap << "_ANIMATION_FRAMES " << images.size() << "\n\n";
        }

        for (unsigned int i = 0; i < images.size(); i++)
        {
            images[i] = ConvertToGBA(images[i]);
            WriteOut(file_c, file_h, images[i], params);
        }


        if (params.animated)
        {
            file_h << "extern const unsigned short* " << params.name << "_frames[" << images.size() << "];\n\n";
            file_c << "const unsigned short* " << params.name << "_frames[" << images.size() << "] =\n{\n\t";
            int space_counter = 0;
            for (unsigned int i = 0; i < images.size(); i++)
            {
                std::string name = images[i].comment();
                Chop(name);
                int last = name.rfind('.');
                name = name.substr(0, last);

                file_c << name << ", ";
                space_counter++;
                if (space_counter == 10)
                {
                    space_counter = 0;
                    file_c << "\n";
                }
            }

            file_c << "\n};\n\n";
        }

        file_h << "#endif";

        file_c.close();
        file_h.close();
    }
    catch ( Exception &error_ )
    {
        printf("%s\n", error_.what());
        printf("Batch Image to GBA (Mode3) failed!");
        exit(EXIT_FAILURE);
    }
}

static void InitFiles(ofstream& file_c, ofstream& file_h, const ExportParams& params)
{
    std::string filename_c = params.name + ".c";
    std::string filename_h = params.name + ".h";
    std::string name = params.name;
    Chop(name);
    std::string name_cap = name;
    transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

    file_c.open(filename_c.c_str());
    file_h.open(filename_h.c_str());

    if (!file_c.good() || !file_h.good())
    {
        printf("Could not open files for writing\n");
        exit(EXIT_FAILURE);
    }

    header.Write(file_c);
    header.Write(file_h);
    file_h << "#ifndef " << name_cap << "_BITMAP_H\n";
    file_h << "#define " << name_cap << "_BITMAP_H\n\n";
}

static void WriteOut(ofstream& file_c, ofstream& file_h, Magick::Image image, const ExportParams& params)
{
    std::string name = image.comment();
    Chop(name);
    int last = name.rfind('.');
    name = name.substr(0, last);

    // If this is part of a multi-image append frame number;
    if (image.label()[0] == 'T')
    {
        std::ostringstream ap;
        ap << name << image.scene();
        name = ap.str();
    }

    std::string name_cap = name;
    transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

    int num_pixels = image.rows() * image.columns();
    char* pixels = new char[num_pixels * sizeof(char) * 3];
    image.write(0, 0, image.columns(), image.rows(), "RGB", CharPixel, pixels);
    int size = 3 * num_pixels;
    char buffer[7];
    int spacecounter = 0;

    file_c << "const unsigned short " << name << "[" << num_pixels << "] =\n{\n\t";

    for (int i = 0; i < size; i += 3)
    {
        // PACKING
        // pixels[i] = r pixels[i+1] = g pixels[i+2] = b
        // Convert to gba format here
        unsigned short byte = pixels[i] | (pixels[i+1] << 5) | (pixels[i+2] << 10);
        sprintf(buffer, "0x%04x", byte);
        file_c << buffer;
        if (i != size - 3)
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

    file_h << "extern const unsigned short " << name << "[" << num_pixels << "];\n";
    file_h << "#define " << name_cap << "_WIDTH " << image.columns() << "\n";
    file_h << "#define " << name_cap << "_HEIGHT " << image.rows() << "\n\n";

    delete[] pixels;
}
