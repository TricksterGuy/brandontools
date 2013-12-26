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

#include "tile.hpp"

#define TOTAL_TILE_MEMORY_BYTES 65536
#define SIZE_CBB_BYTES (8192 * 2)
#define SIZE_SBB_BYTES (1024 * 2)
#define SIZE_SBB_SHORTS 1024

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
        Palette(const std::vector<Color>& _colors, const std::string& _name);
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

class Tileset
{
    public:
        Tileset(const std::vector<Image16Bpp>& images, const std::string& name, int bpp);
        Tileset(const Image16Bpp& image, int bpp);
        int Search (const GBATile& tile) const;
        int Search (const ImageTile& tile) const;
        unsigned int Size() const {return tiles.size() * ((bpp == 4) ? TILE_SIZE_SHORTS_4BPP : (bpp == 8) ? TILE_SIZE_SHORTS_8BPP : 1);};
        void WriteData(std::ostream& file) const;
        void WriteExport(std::ostream& file) const;
        std::string name;
        int bpp;
        // Only one of two will be used bpp = 4 or 8: tiles 16: itiles
        std::set<GBATile> tiles;
        std::set<ImageTile> itiles;
        // Only one max will be used bpp = 4: paletteBanks 8: palette 16: neither
        std::shared_ptr<Palette> palette;
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

#endif
