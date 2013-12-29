#ifndef REDUCTION_HELPER_HPP
#define REDUCTION_HELPER_HPP

#include <iterator>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <Magick++.h>

#define TILE_SIZE 64
#define PALETTE_SIZE 16
#define TILE_SIZE_BYTES_8BPP 64
#define TILE_SIZE_BYTES_4BPP 32
#define TILE_SIZE_SHORTS_8BPP 32
#define TILE_SIZE_SHORTS_4BPP 16
#define TOTAL_TILE_MEMORY_BYTES 65536
#define SIZE_CBB_BYTES (8192 * 2)
#define SIZE_SBB_BYTES (1024 * 2)
#define SIZE_SBB_SHORTS 1024

#include "color.hpp"

class Image16Bpp
{
    public:
        Image16Bpp(Magick::Image image, const std::string& _name);
        void GetColors(std::vector<Color>& colors) const;
        void GetColors(std::vector<Color>::iterator& color_ptr) const;
        unsigned int Size() const {return width * height;};
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        unsigned int width;
        unsigned int height;
        std::string name;
        std::vector<unsigned short> pixels;
};

class Palette
{
    public:
        Palette() {}
        Palette(const std::vector<Color>& _colors, const std::string& _name);
        void Set(const std::vector<Color>& _colors);
        Color At(int index) const {return colors[index];}
        int Search(const Color& color) const;
        int Search(unsigned short color) const;
        unsigned int Size() const {return colors.size();};
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::vector<Color> colors;
        std::string name;
    private:
        mutable std::map<Color, int> colorIndexCache;
};

class Image8Bpp
{
    public:
        Image8Bpp() {};
        Image8Bpp(const Image16Bpp& image);
        void Set(const Image16Bpp& image, std::shared_ptr<Palette> global_palette);
        unsigned int Size() const {return width * height / 2;};
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        unsigned int width;
        unsigned int height;
        std::string name;
        std::vector<unsigned char> pixels;
        std::shared_ptr<Palette> palette;
};

class Image8BppScene
{
    public:
        Image8BppScene(const std::vector<Image16Bpp>& images, const std::string& name);
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::string name;
        std::vector<Image8Bpp> images;
        std::shared_ptr<Palette> palette;
};

class PaletteBank
{
    public:
        PaletteBank(int _id) : id(_id) {};
        PaletteBank(const std::vector<Color>& _colors, int id);
        bool Contains(const Palette& palette) const;
        int CanMerge(const Palette& palette) const;
        void Merge(const Palette& palette);
        void Add(const Color& c);
        unsigned int Size() const {return colors.size();};
        int Search(const Color& c) const;
        std::vector<Color> colors;
        std::set<Color> colorSet;
        int id;
};

std::ostream& operator<<(std::ostream& file, const PaletteBank& bank);
void WriteExportPaletteBanks(std::ostream& file, const std::string& name, const std::vector<PaletteBank>& banks);
void WriteDataPaletteBanks(std::ostream& file, const std::string& name, const std::vector<PaletteBank>& banks);

template <class T>
class Tile
{
    public:
        typedef Tile<unsigned char> GBATile;
        typedef Tile<unsigned short> ImageTile;

        Tile();
        Tile(const Tile<T>& tile);
        Tile(const std::vector<T>& image, int pitch, int tilex, int tiley, int border = 0, int bpp = 8);
        Tile(const T* image, int pitch, int tilex, int tiley, int border = 0, int bpp = 8);
        Tile(std::shared_ptr<ImageTile> imageTile, int bpp);
        ~Tile() {};
        void Set(const std::vector<T>& image, int pitch, int tilex, int tiley, int border = 0, int bpp = 8);
        void Set(const T* image, int pitch, int tilex, int tiley, int border = 0, int bpp = 8);
        void UsePalette(const PaletteBank& bank);
        bool IsEqual(const Tile<T>& other) const;
        bool IsSameAs(const Tile<T>& other) const;
        bool operator<(const Tile<T>& other) const;
        bool operator==(const Tile<T>& other) const;
        int id;
        std::vector<T> data;
        int bpp;
        unsigned char palette_bank;
        Palette palette;
        std::shared_ptr<ImageTile> sourceTile;

    template <class U>
    friend std::ostream& operator<<(std::ostream& file, const Tile<U>& tile);
};

typedef Tile<unsigned char> GBATile;
typedef Tile<unsigned short> ImageTile;

class Tileset
{
    public:
        Tileset(const std::vector<Image16Bpp>& images, const std::string& name, int bpp);
        Tileset(const Image16Bpp& image, int bpp);
        int Search(const GBATile& tile) const;
        int Search(const ImageTile& tile) const;
        // Match Imagetile to GBATile (only for bpp = 4)
        bool Match(const ImageTile& tile, int& tile_id, int& pal_id) const;
        unsigned int Size() const {return tiles.size() * ((bpp == 4) ? TILE_SIZE_SHORTS_4BPP : (bpp == 8) ? TILE_SIZE_SHORTS_8BPP : 1);};
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::string name;
        int bpp;
        // Only one of two will be used bpp = 4 or 8: tiles 16: itiles
        std::set<GBATile> tiles;
        std::set<ImageTile> itiles;
        // Bookkeeping matcher used when bpp = 4 or 8
        std::map<ImageTile, GBATile> matcher;
        // Tiles sorted by id for export.
        std::vector<GBATile> tilesExport;
        // Only one max will be used bpp = 4: paletteBanks 8: palette 16: neither
        std::shared_ptr<Palette> palette;
        std::vector<PaletteBank> paletteBanks;
        // Only valid for bpp = 4 and 8
        std::vector<int> offsets;
    private:
        void Init4bpp(const std::vector<Image16Bpp>& images);
        void Init8bpp(const std::vector<Image16Bpp>& images);
        void Init16bpp(const std::vector<Image16Bpp>& images);
};

class Map
{
    public:
        Map() {};
        Map(const Image16Bpp& image, int bpp);
        Map(const Image16Bpp& image, std::shared_ptr<Tileset> global_tileset);
        void Set(const Image16Bpp& image, std::shared_ptr<Tileset> global_tileset);
        unsigned int Size() const {return data.size();};
        unsigned int Type() const {return (width > 32 ? 1 : 0) | (height > 32 ? 1 : 0) << 1;};
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        unsigned int width;
        unsigned int height;
        std::string name;
        std::vector<unsigned short> data;
        std::shared_ptr<Tileset> tileset;
    private:
        void Init4bpp(const Image16Bpp& image);
        void Init8bpp(const Image16Bpp& image);
};

class MapScene
{
    public:
        MapScene(const std::vector<Image16Bpp>& images, const std::string& name, int bpp);
        MapScene(const std::vector<Image16Bpp>& images, const std::string& name, std::shared_ptr<Tileset> tileset);
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::string name;
        std::vector<Map> maps;
        std::shared_ptr<Tileset> tileset;
};

template <class T>
Tile<T>::Tile() : id(0), data(TILE_SIZE), bpp(8), palette_bank(0)
{
}

template <class T>
Tile<T>::Tile(const Tile<T>& tile) : id(tile.id), data(tile.data), bpp(tile.bpp), palette_bank(tile.palette_bank), palette(tile.palette), sourceTile(tile.sourceTile)
{
}

template <class T>
Tile<T>::Tile(std::shared_ptr<ImageTile> imageTile, int bpp)
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
void Tile<T>::UsePalette(const PaletteBank& bank)
{
    printf("Not implemented");
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
    return data < other.data;
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
