#ifndef SHARED_HPP
#define SHARED_HPP

#include <string>
#include <vector>

#include <Magick++.h>

#include "headerfile.hpp"
#include "implementationfile.hpp"
#include "reductionhelper.hpp"

struct ExportParams
{
    // General Export stuff
    int mode;
    std::string name; // Filename of the exported file + array name if not batch.
    std::vector<std::string> files;
    std::vector<std::string> names; // In batch names of the arrays for the images.
    std::vector<Magick::Image> images;
    std::vector<Magick::Image> tileset;

    // Optional stuff
    unsigned short transparent_color;
    bool transparent_given;
    int width;
    int height;
    bool animated;

    // Palette options
    unsigned int offset;
    int weights[4];
    bool dither;
    float dither_level;
    unsigned int palette;
    bool fullpalette;
    bool split;
    int bpp;

    // Tile/map stuff
    int split_sbb;
    int border;
    bool force;
    bool reduce;

    // Sprite stuff
    bool for_bitmap;
    bool export_2d;
};

void DoMode0(const std::vector<Image16Bpp>& images);
void DoMode3(const std::vector<Image16Bpp>& images);
void DoMode4(const std::vector<Image16Bpp>& images);
void DoTilesetExport(const std::vector<Image16Bpp>& images);
void DoMapExport(const std::vector<Image16Bpp>& images, const std::vector<Image16Bpp>& tilesets);
void DoSpriteExport(const std::vector<Image16Bpp>& images);

#define WARNING_WIDTH "\
Warning: Image (%s) is too wide to fit on the gameboy screen\n\
Please use the resize option if you did not mean this\n\
Image width given %d\n"

#define WARNING_HEIGHT "\
Warning: Image (%s) is too tall to fit on the gameboy screen\n\
Please use the resize option if you did not mean this\n\
Image height given %d\n"

std::string ToUpper(const std::string& str);
void split(const std::string& s, char delimiter, std::vector<std::string>& tokens);
void Chop(std::string& filename);
std::string Sanitize(const std::string& filename);
std::string Format(const std::string& filename);
unsigned int log2(unsigned int x);

extern ExportParams params;

#endif
