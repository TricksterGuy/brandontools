#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include "shared.hpp"
#include "fileutils.hpp"

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
    catch (Exception &error_)
    {
        printf("%s\n", error_.what());
        printf("Image to GBA (Mode3) failed!");
        exit(EXIT_FAILURE);
    }
}

static void WriteC(Magick::Image image, const ExportParams& params)
{
    ofstream file_c, file_h;
    InitFiles(file_c, file_h, params.name);

    std::string name = Format(params.name);
    std::string name_cap = name;
    transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

    // Get Image Data
    int num_pixels = image.rows() * image.columns();
    char* pixels = new char[num_pixels * sizeof(char) * 3];
    image.write(0, 0, image.columns(), image.rows(), "RGB", CharPixel, pixels);

    // Write out .c file
    header.Write(file_c);
    WriteShortArray(file_c, name, "", pixels, num_pixels, PackPixels, 10);
    delete[] pixels;
    file_c.close();

    // Write out .h file
    header.Write(file_h);
    WriteHeaderGuard(file_h, name_cap, "_BITMAP_H");
    WriteExternShortArray(file_h, name, "", num_pixels);
    WriteNewLine(file_h);
    if (params.transparent_color != -1)
    {
        char buffer[7];
        sprintf(buffer, "0x%04x", (unsigned short) params.transparent_color);
        WriteDefine(file_h, name_cap, "_TRANSPARENT", buffer);
    }
    WriteDefine(file_h, name_cap, "_WIDTH", image.columns());
    WriteDefine(file_h, name_cap, "_HEIGHT", image.rows());
    WriteNewLine(file_h);
    WriteEndHeaderGuard(file_h);
    file_h.close();
}
