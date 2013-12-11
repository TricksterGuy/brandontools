#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include "shared.hpp"
#include "tile.hpp"
#include "reductionhelper.hpp"

using namespace Magick;
using namespace std;

static void WriteC(Image image, const ExportParams& params);
static void WriteMap(ostream& file, const ExportParams& params, const std::string& name,
                     const std::vector<unsigned short>& mapData, unsigned int width,
                     unsigned int height, int type);

void DoMode0_8bpp(Magick::Image image, const ExportParams& params)
{
    header.SetMode(0);
    try
    {
        if (image.rows() % 8 != 0 || image.columns() % 8 != 0)
        {
            printf("[ERROR] Map image's dimensions must be divisible by 8. Please fix.\n");
            exit(EXIT_FAILURE);
        }
        if (image.rows() > 512 || image.columns() > 512)
        {
            printf("[ERROR] Map image dimensions must not be greater than 512. Please fix.\n");
            exit(EXIT_FAILURE);
        }
        image = ConvertToGBA(image);
        WriteC(image, params);
    }
    catch (Magick::Exception &error_)
    {
        printf("%s\n", error_.what());
        printf("[ERROR] Image to GBA (Mode0 8bpp) failed!");
        exit(EXIT_FAILURE);
    }
}

static void WriteC(Image image, const ExportParams& params)
{
    unsigned int tilesX = image.columns() / 8;
    unsigned int tilesY = image.rows() / 8;
    unsigned int totalTiles = tilesX * tilesY;
    int type = (tilesX > 32 ? 1 : 0) | (tilesY > 32 ? 1 : 0) << 1;
    int num_blocks = (type == 0 ? 1 : (type < 3 ? 2 : 4));

    unsigned int num_pixels = image.rows() * image.columns();
    std::vector<unsigned char> indexedImage(num_pixels, 1);
    QuantizeImage(image, params, indexedImage);
    std::set<Tile> tileSet;
    std::vector<Tile> tiles;
    std::vector<unsigned short> mapData(totalTiles);

    unsigned int num_colors = params.offset + palette.size();
    // Error check for p_offset
    if (num_colors > 256)
    {
        printf("[ERROR] too many colors in palette. Got %u.\n", num_colors);
        exit(EXIT_FAILURE);
    }
    if (params.fullpalette) num_colors = 256;

    // If not full map add the null tile.  It shall come first.
    if ((tilesX != 32 && tilesX != 64) || (tilesY != 32 && tilesY != 64))
    {
        tiles.push_back(NULLTILE);
        tileSet.insert(NULLTILE);
    }

    // Perform reduce.
    for (unsigned int i = 0; i < totalTiles; i++)
    {
        int tilex = i % tilesX;
        int tiley = i / tilesX;
        Tile tile(indexedImage, image.columns(), tilex, tiley);
        std::set<Tile>::iterator foundTile = tileSet.find(tile);
        if (foundTile != tileSet.end())
            mapData[i] = foundTile->id;
        else
        {
            tile.id = tiles.size();
            tiles.push_back(tile);
            tileSet.insert(tile);
            mapData[i] = tile.id;
        }
    }

    // Checks
    int memory_b = tiles.size() * TILE_SIZE_BYTES_8BPP + num_blocks * SIZE_SBB_BYTES;
    if (tiles.size() >= 1024)
    {
        printf("[ERROR] Too many tiles found %d tiles.\n"
               "Please use -reduce or make the image simpler.\n", totalTiles);
        exit(EXIT_FAILURE);
    }
    if (memory_b > TOTAL_TILE_MEMORY_BYTES)
    {
        printf("[ERROR] Memory required for tiles + map exceeds %d bytes, was %d bytes.\n"
               "Please use -reduce or make the image simpler.\n",
               TOTAL_TILE_MEMORY_BYTES, memory_b);
        exit(EXIT_FAILURE);
    }
    // Delicious infos
    int cbbs = tiles.size() * TILE_SIZE_BYTES_8BPP / SIZE_CBB_BYTES;
    int sbbs = (int) ceil(tiles.size() * TILE_SIZE_BYTES_8BPP % SIZE_CBB_BYTES / ((double)SIZE_SBB_BYTES));
    printf("[INFO] Tiles found %zu Map info size %d (shorts) Exported map size %d (shorts).\n",
           tiles.size(), totalTiles, num_blocks * SIZE_SBB_SHORTS);
    printf("[INFO] Tiles uses %d charblocks and %d screenblocks, Map uses %d screenblocks.\n",
           cbbs, sbbs, num_blocks);
    printf("[INFO] Total utilization %.2f/4 charblocks or %d/32 screenblocks, %d/65536 bytes.\n",
           memory_b / ((double)SIZE_CBB_BYTES), (int) ceil(memory_b / ((double)SIZE_SBB_BYTES)), memory_b);

    ofstream file_c, file_h;
    InitFiles(file_c, file_h, params.name);
    std::string name = Format(params.name);
    std::string name_cap = name;
    transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

    // Write Header information
    header.Write(file_c);
    header.Write(file_h);

    // Write Header file
    WriteHeaderGuard(file_h, name_cap, "_TILEMAP_H");
    WriteExternShortArray(file_h, name, "_palette", num_colors);
    if (params.offset)
        WriteDefine(file_h, name_cap, "_PALETTE_OFFSET ", params.offset);
    WriteDefine(file_h, name_cap, "_PALETTE_SIZE", num_colors);
    WriteDefine(file_h, name_cap, "_PALETTE_TYPE", "(1 << 7)");
    WriteNewLine(file_h);

    WriteExternShortArray(file_h, name, "_map", num_blocks * SIZE_SBB_SHORTS);
    WriteDefine(file_h, name_cap, "_WIDTH", image.columns() / 8);
    WriteDefine(file_h, name_cap, "_HEIGHT", image.rows() / 8);
    WriteDefine(file_h, name_cap, "_MAP_SIZE", num_blocks * SIZE_SBB_SHORTS);
    WriteDefine(file_h, name_cap, "_MAP_TYPE", type << 14);
    WriteNewLine(file_h);

    WriteExternShortArray(file_h, name, "_tiles", tiles.size() * TILE_SIZE_SHORTS_8BPP);
    WriteDefine(file_h, name_cap, "_TILES", tiles.size() * TILE_SIZE_SHORTS_8BPP);
    WriteEndHeaderGuard(file_h);

    // Write Palette Data
    WriteShortArray(file_c, name, "_palette", palette.data(), num_colors, GetPaletteEntry, 10, &params.offset);
    // Write Map Data
    WriteMap(file_c, params, name, mapData, tilesX, tilesY, type);
    file_c << "\n";
    // Write Tile Data
    WriteTiles(file_c, params, name, tiles);
    file_c << "\n";
}

void WriteMap(ostream& file, const ExportParams& params, const std::string& name,
              const std::vector<unsigned short>& mapData, unsigned int width, unsigned int height, int type)
{
    int num_blocks = (type == 0 ? 1 : (type < 3 ? 2 : 4));
    file << "const unsigned short " << name << "_map[" << num_blocks * 32 * 32 << "] =\n{\n\t";

    for (int i = 0; i < num_blocks; i++)
    {
        // Case for each possible value of num_blocks
        // 1: 0
        // 2: type is 1 - 0, 1
        //    type is 2 - 0, 2
        // 4: 0, 1, 2, 3
        int sbb = (i == 0 ? 0 : (i == 1 && type == 2 ? 2 : i));
        unsigned int sx, sy;
        sx = ((sbb & 1) != 0) * 32;
        sy = ((sbb & 2) != 0) * 32;
        for (unsigned int y = 0; y < 32; y++)
        {
            for (unsigned int x = 0; x < 32; x++)
            {
                // Read tile if outside bounds replace with null tile
                unsigned short tile_id;
                if (x + sx > width || y + sy > height)
                    tile_id = 0;
                else
                    tile_id = mapData[(y + sy) * width + (x + sx)];
                // Write it.
                WriteElement(file, tile_id, num_blocks * 32 * 32, (y + sy) * width + (x + sx), 8);
            }
        }
    }
    file << "\n};\n";
}
