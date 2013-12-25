#ifndef TILE_HPP
#define TILE_HPP

#include "color.hpp"

#include <cstdio>
#include <iostream>
#include <set>
#include <vector>

#define TILE_SIZE 64
#define PALETTE_SIZE 16
#define TILE_SIZE_BYTES_8BPP 64
#define TILE_SIZE_BYTES_4BPP 32
#define TILE_SIZE_SHORTS_8BPP 32
#define TILE_SIZE_SHORTS_4BPP 16

template <class T>
class Tile
{
    public:
        Tile() : id(0), data(TILE_SIZE), bpp(8), palette_bank(0) {};
        Tile(const std::vector<T>& image, int pitch, int tilex, int tiley, int border = 0, int bpp = 8);
        Tile(const T* image, int pitch, int tilex, int tiley, int border = 0, int bpp = 8);
        Tile(const Tile<unsigned short>& imageTile, int bpp);
        ~Tile() {};
        void Set(const std::vector<T>& image, int pitch, int tilex, int tiley, int border = 0, int bpp = 8);
        void Set(const T* image, int pitch, int tilex, int tiley, int border = 0, int bpp = 8);
        bool IsEqual(const Tile<T>& other) const;
        bool IsSameAs(const Tile<T>& other) const;
        bool operator<(const Tile<T>& other) const;
        bool operator==(const Tile<T>& other) const;
        const std::vector<T>& GetData() const;
        const std::set<Color>& GetPalette() const;
        int id;
    private:
        std::vector<T> data;
        int bpp;
        unsigned char palette_bank;
        std::set<Color> palette;

    template <class U>
    friend std::ostream& operator<<(std::ostream& file, const Tile<U>& tile);
};

typedef Tile<unsigned char> GBATile;
typedef Tile<unsigned short> ImageTile;

template <class T>
Tile<T>::Tile(const Tile<unsigned short>& imageTile, int bpp)
{
    printf("Not implemented");
}

template <class T>
Tile<T>::Tile(const std::vector<T>& image, int pitch, int tilex, int tiley, int border, int bpp) : data(TILE_SIZE)
{
    Set(image, pitch, tilex, tiley, border, bpp);
}

template <class T>
Tile<T>::Tile(const T* image, int pitch, int tilex, int tiley, int border, int bpp) : data(TILE_SIZE)
{
    Set(image, pitch, tilex, tiley, border, bpp);
}

template <class T>
void Tile<T>::Set(const std::vector<T>& image, int pitch, int tilex, int tiley, int border, int bpp)
{
    this->bpp = bpp;
    T* ptr = data.data();

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ptr[i * 8 + j] = image[(tiley * (8+border) + i) * pitch + tilex * (8+border) + j];
        }
    }
}

template <class T>
void Tile<T>::Set(const T* image, int pitch, int tilex, int tiley, int border, int bpp)
{
    this->bpp = bpp;
    T* ptr = data.data();

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ptr[i * 8 + j] = image[(tiley * (8+border) + i) * pitch + tilex * (8+border) + j];
        }
    }
}

template <class T>
bool Tile<T>::IsEqual(const Tile<T>& other) const
{
    return data == other.data;
}

template <class T>
bool Tile<T>::operator==(const Tile<T>& other) const
{
    return IsEqual(other);
}

template <class T>
bool Tile<T>::operator<(const Tile<T>& other) const
{
    return id < other.id;
}

template <class T>
bool Tile<T>::IsSameAs(const Tile<T>& other) const
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

template <class T>
const std::vector<T>& Tile<T>::GetData() const
{
    return data;
}

template <class T>
const std::set<Color>& Tile<T>::GetPalette() const
{
    return palette;
}

template <class T>
std::ostream& operator<<(std::ostream& file, const Tile<T>& tile)
{

    file << std::hex;
    for (unsigned int i = 0; i < 8; i++)
    {
        for (unsigned int j = 0; j < 8; j++)
            file << tile.data[i * 8 + j] << " ";
        file << std::endl;
    }
    file << std::dec;
    return file;
}

bool TilesPaletteSizeComp(const GBATile& i, const GBATile& j);

#endif
