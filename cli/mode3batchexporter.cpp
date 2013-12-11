#include <Magick++.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "shared.hpp"
#include "reductionhelper.hpp"

using namespace Magick;
using namespace std;

static void WriteC(std::vector<Magick::Image> images, const ExportParams& params);
static void WriteOut(ofstream& file_c, ofstream& file_h, unsigned int index, Magick::Image image, const ExportParams& params);

void DoMode3Multi(std::vector<Magick::Image> images, const ExportParams& params)
{
    header.SetMode(3);
    try
    {
        WriteC(images, params);
    }
    catch ( Exception &error_ )
    {
        printf("%s\n", error_.what());
        printf("Batch Image to GBA (Mode3) failed!");
        exit(EXIT_FAILURE);
    }
}

void WriteC(std::vector<Magick::Image> images, const ExportParams& params)
{
    std::string name = Format(params.name);
    std::string name_cap = ToUpper(name);

    ofstream file_c, file_h;
    InitFiles(file_c, file_h, params.name);

    header.Write(file_c);
    header.Write(file_h);

    WriteHeaderGuard(file_h, name_cap, "_BITMAP_H");
    if (params.animated)
    {
        WriteExternShortPtrArray(file_h, name, "_frames", images.size());
        WriteDefine(file_h, name_cap, "_ANIMATION_FRAMES", images.size());
        WriteNewLine(file_h);
        WriteNewLine(file_h);

        WriteShortPtrArray(file_c, name, "_frames", params.names.data(), params.names.size(), getAnimFrameName, 1);
        WriteNewLine(file_c);
    }

    if (params.transparent_color != -1)
    {
        char buffer[7];
        sprintf(buffer, "0x%04x", (unsigned short) params.transparent_color);
        WriteDefine(file_h, name_cap, "_TRANSPARENT", buffer);
        WriteNewLine(file_h);
        WriteNewLine(file_h);
    }

    for (unsigned int i = 0; i < images.size(); i++)
        WriteOut(file_c, file_h, i, images[i], params);

    WriteEndHeaderGuard(file_h);
    WriteNewLine(file_h);
    WriteNewLine(file_c);

    file_c.close();
    file_h.close();
}

void WriteOut(ofstream& file_c, ofstream& file_h, unsigned int index, Magick::Image image, const ExportParams& params)
{
    std::string name = params.names[index];
    std::string name_cap = ToUpper(name);

    image = ConvertToGBA(image);

    int num_pixels = image.rows() * image.columns();
    char* pixels = new char[num_pixels * sizeof(char) * 3];
    image.write(0, 0, image.columns(), image.rows(), "RGB", CharPixel, pixels);

    WriteShortArray(file_c, name, "", pixels, num_pixels, PackPixels, 10);
    WriteNewLine(file_c);
    delete[] pixels;

    WriteExternShortArray(file_h, name, "", num_pixels);
    WriteDefine(file_h, name, "_WIDTH", image.columns());
    WriteDefine(file_h, name, "_HEIGHT", image.rows());
    WriteNewLine(file_h);
}
