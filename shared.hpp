#ifndef SHARED_HPP
#define SHARED_HPP

#include "version.h"
#include "mediancut.hpp"
#include <Magick++.h>
#include <cmath>

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
    private:
        std::vector<std::string> lines;
        std::vector<std::string> images;
        std::string invocation;
        int p_color;
        int mode;
};

struct ExportParams
{
    // General Export stuff
    std::string name;

    // Optional stuff
    int transparent_color;
    int width;
    int height;
    bool animated;

    // Palette options
    int offset;
    int weights[4];
    bool dither;
    float dither_level;
    unsigned int palette;
    bool fullpalette;
    bool split;

};


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

extern Header header;
extern std::vector<Color> palette;

int paletteSearch(Color a);
Magick::Image ConvertToGBA(Magick::Image image);
void split(const std::string& s, char delimiter, std::vector<std::string>& tokens);
void RiemersmaDither(std::vector<Color>::iterator image, std::vector<int>& indexedImage, int width, int height, int dither, float ditherlevel);
void Chop(std::string& filename);

#endif
