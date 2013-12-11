#include "tile.hpp"
#include "shared.hpp"
#include <cstdlib>
#include <cstring>

Tile NULLTILE;

Tile::Tile(unsigned char* data_ptr, unsigned short* palette_ptr, unsigned short tile_id) :
    id(tile_id), data(TILE_SIZE), palette(PALETTE_SIZE)
{
    Set(data_ptr, palette_ptr);
}

Tile::Tile(const std::vector<unsigned char>& indexedImage, int pitch, int tilex, int tiley, int border, bool is_8bpp) :
    data(TILE_SIZE), palette(PALETTE_SIZE)
{
    Set(indexedImage, pitch, tilex, tiley, border, is_8bpp);
}

Tile::Tile(const std::vector<Color>& image, int pitch, int tilex, int tiley, int border)
{
    Set(image, pitch, tilex, tiley, border);
}

Tile::~Tile()
{
}

void Tile::Set(unsigned char* data_ptr, unsigned short* palette_ptr)
{
    is_8bpp = true;
    if (data_ptr)
        data.assign(data_ptr, data_ptr + TILE_SIZE);
    if (palette_ptr)
    {
        is_8bpp = false;
        palette.assign(palette_ptr, palette_ptr + PALETTE_SIZE);
    }
}

void Tile::Set(const std::vector<unsigned char>& indexedImage, int pitch, int tilex, int tiley, int border, bool is_8bpp_data)
{
    is_8bpp = is_8bpp_data;
    unsigned char* ptr = data.data();

    for (int i = 0; i < 8; i++)
        memcpy(ptr + i * 8, indexedImage.data() + (tiley * (8+border) + i) * pitch + tilex * (8+border), 8);
}

void Tile::Set(const std::vector<Color>& image, int pitch, int tilex, int tiley, int border)
{
    is_8bpp = false;
    std::vector<Color> tile_data(TILE_SIZE);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            tile_data[i * 8 + j] = image[(tiley * (8+border) + i) * pitch + tilex * (8+border) + j];
}

bool Tile::IsEqual(const Tile& other) const
{
    return data == other.data && palette == other.palette;
}

bool Tile::operator==(const Tile& other) const
{
    return IsEqual(other);
}

bool Tile::IsSameAs(const Tile& other) const
{
    bool same, sameh, samev, samevh;
    same = sameh = samev = samevh = true;
    for (int i = 0; i < TILE_SIZE; i++)
    {
        int x = i % 8;
        int y = i / 8;
        same = same && data[i] == other.data[i];
        samev = samev && data[i] == other.data[(7 - y) * 8 + x];
        sameh = sameh && data[i] == other.data[y * 8 + (7 - x)];
        samevh = samevh && data[i] == other.data[(7 - y) * 8 + (7 - x)];
    }
    return same || samev || sameh || samevh;
}

bool Tile::operator<(const Tile& other) const
{
    return data < other.data;
}

std::ostream& operator<<(std::ostream& file, const Tile& tile)
{
    if (tile.is_8bpp)
    {
        for (unsigned int i = 0; i < TILE_SIZE_SHORTS_8BPP; i++)
        {
            int px1 = tile.data[2 * i];
            int px2 = tile.data[2 * i + 1];
            unsigned short data = px1 | (px2 << 8);
            WriteElement(file, data, TILE_SIZE_SHORTS_8BPP, i, 8);
        }
    }
    else
    {
        for (unsigned int i = 0; i < TILE_SIZE_SHORTS_4BPP; i++)
        {
            int px1 = tile.data[4 * i];
            int px2 = tile.data[4 * i + 1];
            int px3 = tile.data[4 * i + 2];
            int px4 = tile.data[4 * i + 3];
            unsigned short data = (px4 << 12) | (px3 << 8) | (px2 << 4) | px1;
            WriteElement(file, data, TILE_SIZE_SHORTS_4BPP, i, 8);
        }
    }
    return file;
}
