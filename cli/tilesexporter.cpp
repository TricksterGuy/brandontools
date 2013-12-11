#include <Magick++.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "shared.hpp"
#include "tile.hpp"
#include "reductionhelper.hpp"

using Magick::Image;
using namespace std;


static void WriteC(std::vector<Magick::Image> images, const ExportParams& params);
static void WriteAll(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params);
static void WriteSeparate(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params);

void DoTilesetExport(std::vector<Magick::Image> images, const ExportParams& params)
{
    header.SetMode(0);
    try
    {
        WriteC(images, params);
    }
    catch (Magick::Exception &error_)
    {
        printf("%s\n", error_.what());
        printf("[ERROR] Image to Tiles failed!");
        exit(EXIT_FAILURE);
    }
}

void WriteC(std::vector<Magick::Image> images, const ExportParams& params)
{
    ofstream file_c, file_h;
    InitFiles(file_c, file_h, params.name);
    std::string name = Format(params.name);
    int tile_width = 8 + params.border;

    for (unsigned int i = 0; i < images.size(); i++)
    {
        if (images[i].columns() % tile_width != 0 || images[i].rows() % tile_width != 0)
        {
            printf("[ERROR] Tileset image's dimensions must be divisible by 8+%d. Please fix.\n", params.border);
            exit(EXIT_FAILURE);
        }
    }

    // -split allows you to generate multiple palettes/tilesets for each image given
    //if (params.split)
    //	WriteSeparate(file_c, file_h, images, params);
    //else
    WriteAll(file_c, file_h, images, params);

    WriteEndHeaderGuard(file_h);
    WriteNewLine(file_h);
    file_h.close();

    WriteNewLine(file_c);
    file_c.close();
}

void WriteAll(ofstream& file_c, ofstream& file_h, std::vector<Magick::Image> images, const ExportParams& params)
{
    std::string name = Format(params.name);
    std::string name_cap = ToUpper(name);

    std::vector<Tile> tiles;
    std::vector<short> offsets;

    if (params.bpp == 8)
        GetTiles8bpp(images, params, tiles, offsets);
    //else
    //    GetTiles4bpp(images, params, tiles);

    unsigned int num_colors = params.offset + palette.size();
    // Error check for p_offset
    if (num_colors > 256)
    {
        printf("[ERROR] too many colors in palette. Got %u.\n", num_colors);
        exit(EXIT_FAILURE);
    }
    if (params.fullpalette) num_colors = 256;

    // Write Header information
    header.Write(file_c);
    header.Write(file_h);

    // Write Header file
    WriteHeaderGuard(file_h, name_cap, "_TILEMAP_H");
    WriteExternShortArray(file_h, name, "_palette", num_colors);
    if (params.offset)
        WriteDefine(file_h, name_cap, "_PALETTE_OFFSET ", params.offset);
    WriteDefine(file_h, name_cap, "_PALETTE_SIZE", num_colors);
    WriteDefine(file_h, name_cap, "_PALETTE_TYPE", params.bpp == 8 ? "(1 << 7)" : "(0 << 7)");
    WriteNewLine(file_h);

    WriteExternShortArray(file_h, name, "_tiles", tiles.size() * TILE_SIZE_SHORTS_8BPP);
    WriteDefine(file_h, name_cap, "_TILES", tiles.size());
    WriteDefine(file_h, name_cap, "_TILES_SIZE", tiles.size() * TILE_SIZE_SHORTS_8BPP);
    if (images.size() > 1)
    {
        for (unsigned int i = 0; i < images.size(); i++)
        {
            std::string name_cap = ToUpper(params.names[i]);
            WriteDefine(file_h, name_cap, "_TILE_OFFSET", offsets[i]);
        }

    }

    // Write Palette Data
    WriteShortArray(file_c, name, "_palette", palette.data(), num_colors, GetPaletteEntry, 10, &params.offset);

    // Write Tile Data
    WriteTiles(file_c, params, name, tiles);
}
