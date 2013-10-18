#ifndef PALETTE_GEN_HPP
#define PALETTE_GEN_HPP

#include <Magick++.h>
#include <vector>

enum
{
    PALETTE_GEN_NONE,
    PALETTE_GEN_IM,
    PALETTE_GEN_MC,
    PALETTE_GEN_HI,
};

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

typedef std::vector<Color> Palette;

void PaletteGenIM();
void PaletteGenMC();
void PaletteGenHI();
void PaletteGenNone();

#endif
