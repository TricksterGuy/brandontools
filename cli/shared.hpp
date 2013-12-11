#ifndef SHARED_HPP
#define SHARED_HPP

#include "version.h"
#include "fileutils.hpp"
#include <Magick++.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

class Header
{
    public:
        Header(const std::string& invocation = "");
        ~Header();
        void Write(std::ostream& file);
        void AddLine(const std::string& line);
        void AddImage(const Magick::Image& image, bool frame = false);
        void SetInvocation(const std::string& invo);
        void SetTransparent(int p_color);
        void SetMode(int mode);
        void SetTilesets(const std::vector<std::string>& tilesets);
    private:
        std::vector<std::string> lines;
        std::vector<std::string> images;
        std::vector<std::string> tilesets;
        std::string invocation;
        int p_color;
        int mode;
};

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
    int transparent_color;
    int width;
    int height;
    bool animated;
    int bpp;
    bool reduce;

    // Palette options
    unsigned int offset;
    int weights[4];
    bool dither;
    float dither_level;
    unsigned int palette;
    bool fullpalette;
    bool split;

    // Tile/map stuff
    int split_sbb;
    int border;
};

extern Header header;

void DoMode0_4bpp(Magick::Image image, const ExportParams& params);
void DoMode0_8bpp(Magick::Image image, const ExportParams& params);
void DoTilesetExport(std::vector<Magick::Image> images, const ExportParams& params);
void DoMapExport(Magick::Image, const ExportParams& params);
void DoMode3(Magick::Image image, const ExportParams& params);
void DoMode4(Magick::Image image, const ExportParams& params);
void DoMode3Multi(std::vector<Magick::Image> images, const ExportParams& params);
void DoMode4Multi(std::vector<Magick::Image> images, const ExportParams& params);

#define WARNING_WIDTH "\
Warning: Image (%s) is too wide to fit on the gameboy screen\n\
Please use the resize option if you did not mean this\n\
Image width given %d\n"

#define WARNING_HEIGHT "\
Warning: Image (%s) is too tall to fit on the gameboy screen\n\
Please use the resize option if you did not mean this\n\
Image height given %d\n"

#define round(x) (((x) < 0) ? ceil((x) - 0.5) : floor((x) + 0.5))

std::string ToUpper(const std::string& str);
void split(const std::string& s, char delimiter, std::vector<std::string>& tokens);
void Chop(std::string& filename);
std::string Sanitize(const std::string& filename);
std::string Format(const std::string& filename);

#endif
