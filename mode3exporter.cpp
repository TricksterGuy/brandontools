#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include "shared.hpp"

using namespace Magick;
using namespace std;

static void WriteC(Magick::Image image, const ExportParams& params);

void DoMode3(Magick::Image image, const ExportParams& params)
{
    header.SetMode(3);
    try
    {
        image = ConvertToGBA(image);
        WriteC(image, params);
    }
    catch ( Exception &error_ )
    {
        printf("%s\n", error_.what());
        printf("Image to GBA (Mode3) failed!");
        exit(EXIT_FAILURE);
    }
}

static void WriteC(Magick::Image image, const ExportParams& params)
{
    ofstream file_c, file_h;

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

    int num_pixels = image.rows() * image.columns();
    char* pixels = new char[num_pixels * sizeof(char) * 3];
    image.write(0, 0, image.columns(), image.rows(), "RGB", CharPixel, pixels);
    int size = 3 * num_pixels;
    char buffer[7];
    int spacecounter = 0;

    header.Write(file_c);
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
    file_c << "\n};\n";

    header.Write(file_h);
    file_h << "#ifndef " << name_cap << "_BITMAP_H\n";
    file_h << "#define " << name_cap << "_BITMAP_H\n\n";
    file_h << "extern const unsigned short " << name << "[" << num_pixels << "];\n";
    if (params.transparent_color != -1)
        file_h << "#define " << name_cap << "_TRANSPARENT 0x" << std::hex << (unsigned short) params.transparent_color << std::dec << "\n";
    file_h << "#define " << name_cap << "_WIDTH " << image.columns() << "\n";
    file_h << "#define " << name_cap << "_HEIGHT " << image.rows() << "\n\n";
    file_h << "#endif";

    file_c.close();
    file_h.close();

    delete[] pixels;
}
