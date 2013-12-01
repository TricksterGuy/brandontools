#include <Magick++.h>
#include <string>
#include <iostream>
#include <fstream>
#include "shared.hpp"
#include "tile.hpp"

using namespace Magick;
using namespace std;

static void WriteC(Image image, const ExportParams& params);
static void WriteMap(ostream& file, const ExportParams& params, const std::string& name,
                     const std::vector<unsigned short>& mapData, unsigned int width,
                     unsigned int height, int type);
static void WriteTiles(ostream& file, const ExportParams& params, const std::string& name,
                       const std::vector<Tile>& tiles);

void DoMode0_4bpp(Magick::Image image, const ExportParams& params)
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
        printf("[ERROR] Image to GBA (Mode0 4bpp) failed!");
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
    bool expedited = false;

    unsigned int num_colors = params.offset + palette.size();
    // Error check for p_offset
    if (num_colors > 256)
    {
        printf("[ERROR] too many colors in palette. Got %d.\n", num_colors);
        exit(EXIT_FAILURE);
    }
    if (params.fullpalette) num_colors = 256;

    if (num_colors <= 16)
    {
        printf("[INFO] Woohoo expedited processing activated. Image has < 16 colors.\n");
    }


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
        Tile tile(indexedImage, image.columns(), tilex, tiley, false);
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
    int memory_b = tiles.size() * TILE_SIZE_BYTES_4BPP + num_blocks * SIZE_SBB_BYTES;
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
    int cbbs = tiles.size() * TILE_SIZE_BYTES_4BPP / SIZE_CBB_BYTES;
    int sbbs = (int) ceil(tiles.size() * TILE_SIZE_BYTES_4BPP % SIZE_CBB_BYTES / ((double)SIZE_SBB_BYTES));
    printf("[INFO] Tiles found %d Map info size %d (shorts) Exported map size %d (shorts).\n",
           tiles.size(), totalTiles, num_blocks * SIZE_SBB_SHORTS);
    printf("[INFO] Tiles uses %d charblocks and %d screenblocks, Map uses %d screenblocks.\n",
           cbbs, sbbs, num_blocks);
    printf("[INFO] Total utilization %.2f/4 charblocks or %d/32 screenblocks, %d/65536 bytes.\n",
           memory_b / ((double)SIZE_CBB_BYTES), (int) ceil(memory_b / ((double)SIZE_SBB_BYTES)), memory_b);

    ofstream file_c, file_h;
    std::string filename_c = params.name + ".c";
    std::string filename_h = params.name + ".h";
    std::string name = params.name;
    Chop(name);
    name = Sanitize(name);
    std::string name_cap = name;

    transform(name_cap.begin(), name_cap.end(), name_cap.begin(), (int(*)(int)) std::toupper);

    file_c.open(filename_c.c_str());
    file_h.open(filename_h.c_str());

    if (!file_c.good() || !file_h.good())
    {
        printf("[FATAL] Could not open files for writing\n");
        exit(EXIT_FAILURE);
    }

    // Write Header information
    header.Write(file_c);
    header.Write(file_h);

    // Write Header file
    file_h << "#ifndef " << name_cap << "_TILEMAP_H\n";
    file_h << "#define " << name_cap << "_TILEMAP_H\n\n";
    file_h << "extern const unsigned short " << name << "_palette[" << num_colors << "];\n";
    file_h << "extern const unsigned short " << name << "_map[" << num_blocks * SIZE_SBB_SHORTS << "];\n";
    file_h << "extern const unsigned short " << name << "_tiles[" << tiles.size() * TILE_SIZE_SHORTS_4BPP << "];\n\n";
    file_h << "#define " << name_cap << "_PALETTE_SIZE " << num_colors << "\n";
    file_h << "#define " << name_cap << "_PALETTE_TYPE 0 << 7\n";
    if (params.offset)
        file_h << "#define " << name_cap << "_PALETTE_OFFSET " << params.offset << "\n";
    file_h << "#define " << name_cap << "_WIDTH " << image.columns() / 8 << "\n";
    file_h << "#define " << name_cap << "_HEIGHT " << image.rows() / 8 << "\n";
    file_h << "#define " << name_cap << "_MAP_SIZE " << num_blocks * SIZE_SBB_SHORTS << "\n";
    file_h << "#define " << name_cap << "_MAP_TYPE " << type << " << 14" << "\n";
    file_h << "#define " << name_cap << "_TILES " << tiles.size() * TILE_SIZE_SHORTS_4BPP << "\n\n";
    file_h << "#endif";

    // Write Palette Data
    WritePalette(file_c, params, name, num_colors);
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
    file << "const unsigned short " << name << "_map[" << num_blocks * SIZE_SBB_SHORTS << "] =\n{\n\t";

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
                WriteData(file, tile_id, num_blocks * SIZE_SBB_SHORTS, (y + sy) * width + (x + sx), 8);
            }
        }
    }
    file << "\n};\n";
}

void WriteTiles(ostream& file, const ExportParams& params, const std::string& name,
                const std::vector<Tile>& tiles)
{
    file << "const unsigned short " << name << "_tiles[" << tiles.size() * TILE_SIZE_SHORTS_4BPP << "] =\n{\n\t";
    for (unsigned int i = 0; i < tiles.size(); i++)
    {
        file << tiles[i];
        if (i != tiles.size() - 1)
            file << ",\n\t";
    }
    file << "\n};\n";
}
