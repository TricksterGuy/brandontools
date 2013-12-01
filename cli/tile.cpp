#include "tile.hpp"
#include <cstdlib>
#include <cstring>

Tile NULLTILE;

Tile::Tile(unsigned char* data_ptr, unsigned short* palette_ptr, unsigned short tile_id) :
    id(tile_id), data(TILE_SIZE), palette(PALETTE_SIZE)
{
    Set(data_ptr, palette_ptr);
}

Tile::Tile(const std::vector<unsigned char>& indexedImage, int pitch, int tilex, int tiley) : data(TILE_SIZE), palette(PALETTE_SIZE)
{
    Set(indexedImage, pitch, tilex, tiley);
}

Tile::Tile(const std::vector<Color>& image, int pitch, int tilex, int tiley)
{
    Set(image, pitch, tilex, tiley);
}

Tile::~Tile()
{
}

void Tile::Set(unsigned char* data_ptr, unsigned short* palette_ptr)
{
    if (data_ptr)
        data.assign(data_ptr, data_ptr + TILE_SIZE);
    if (palette_ptr)
        palette.assign(palette_ptr, palette_ptr + PALETTE_SIZE);
}

void Tile::Set(const std::vector<unsigned char>& indexedImage, int pitch, int tilex, int tiley)
{
    unsigned char* ptr = data.data();

    for (int i = 0; i < 8; i++)
        memcpy(ptr + i * 8, indexedImage.data() + (tiley * 8 + i) * pitch + tilex * 8, 8);
}

void Tile::Set(const std::vector<Color>& image, int pitch, int tilex, int tiley)
{
    std::vector<Color> tile_data(TILE_SIZE);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            tile_data[i * 8 + j] = image[(tiley * 8 + i) * pitch + tilex * 8 + j];
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
    int spacecounter = 0;
    char buffer[7];
    for (unsigned int i = 0; i < 64; i += 2)
    {
        int px1 = tile.data[i];
        int px2 = tile.data[i + 1];
        unsigned short byte = px1 | (px2 << 8);
        sprintf(buffer, "0x%04x", byte);
        file << buffer;
        if (i != 62)
        {
            file << ",";
            spacecounter++;
            if (spacecounter == 8)
            {
                file << "\n\t";
                spacecounter = 0;
            }
        }
    }
    return file;
}
