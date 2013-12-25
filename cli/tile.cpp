#include "tile.hpp"

#include <map>

#include "fileutils.hpp"
#include "reductionhelper.hpp"

template <>
Tile<unsigned short>::Tile() : id(0), data(TILE_SIZE), bpp(16), palette_bank(0)
{
}

template <>
Tile<unsigned short>::Tile(const std::vector<unsigned short>& image, int pitch, int tilex, int tiley, int border, int ignored) : data(TILE_SIZE), palette_bank(0)
{
    Set(image, pitch, tilex, tiley, border, 16);
}

template <>
Tile<unsigned short>::Tile(const unsigned short* image, int pitch, int tilex, int tiley, int border, int ignored) : data(TILE_SIZE), palette_bank(0)
{
    Set(image, pitch, tilex, tiley, border, 16);
}

template <>
Tile<unsigned char>::Tile(const Tile<unsigned short>& imageTile, int _bpp) : data(TILE_SIZE), bpp(_bpp)
{
/*    // bpp reduce
    int num_colors = 1 << bpp;
    const unsigned short* imgdata = imageTile.GetData().data();
    int weights[4] = {25, 25, 25, 25};
    std::vector<Color> paletteArray;

    std::vector<Color> pixels(TILE_SIZE);

    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        unsigned short pix = imgdata[i];
        pixels[i].Set(pix & 0x1f, (pix >> 5) & 0x1f, (pix >> 10) & 0x1f);
    }

    MedianCut(pixels, num_colors, paletteArray, weights);

    std::map<Color, int> paletteMap;
    for (unsigned int i = 0; i < TILE_SIZE; i++)
        data[i] = paletteSearch(pixels[i], paletteArray, paletteMap);

    for (unsigned int i = 0; i < paletteArray.size(); i++)
        palette.insert(paletteArray[i]);*/
}

template <>
std::ostream& operator<<(std::ostream& file, const Tile<unsigned char>& tile)
{
    const std::vector<unsigned char>& data = tile.GetData();
    if (tile.bpp == 8)
    {
        for (unsigned int i = 0; i < TILE_SIZE_SHORTS_8BPP; i++)
        {
            int px1 = data[2 * i];
            int px2 = data[2 * i + 1];
            unsigned short shrt = px1 | (px2 << 8);
            WriteElement(file, shrt, TILE_SIZE_SHORTS_8BPP, i, 8);
        }
    }
    else
    {
        for (unsigned int i = 0; i < TILE_SIZE_SHORTS_4BPP; i++)
        {
            int px1 = data[4 * i];
            int px2 = data[4 * i + 1];
            int px3 = data[4 * i + 2];
            int px4 = data[4 * i + 3];
            unsigned short shrt = (px4 << 12) | (px3 << 8) | (px2 << 4) | px1;
            WriteElement(file, shrt, TILE_SIZE_SHORTS_4BPP, i, 8);
        }
    }
    return file;
}

bool TilesPaletteSizeComp(const GBATile& i, const GBATile& j)
{
    return i.GetPalette().size() > j.GetPalette().size();
}
