#include "dither.hpp"

#include <algorithm>
#include <cmath>

#ifndef CLAMP
#define CLAMP(x) (((x) < 0.0) ? 0.0 : (((x) > 31) ? 31 : (x)))
#endif

struct DitherImage
{
    DitherImage(std::vector<Color>::iterator i, Image8Bpp& image, int _dither, float _ditherlevel) : pixels(i), output(image), x(0), y(0),
        dither(_dither), ditherlevel(_ditherlevel) {};
    std::vector<Color>::iterator pixels;
    Image8Bpp& output;
    int x, y;
    int dither;
    float ditherlevel;
};

enum
{
    NONE,
    UP,
    LEFT,
    DOWN,
    RIGHT,
};

int Dither(Color& color, std::shared_ptr<Palette> palette, int dither, float ditherlevel)
{
    static int ex = 0, ey = 0, ez = 0;
    Color newColor(CLAMP(color.x + ex), CLAMP(color.y + ey), CLAMP(color.z + ez));
    int index = palette->Search(newColor);
    newColor = palette->At(index);

    if (dither)
    {
        ex += color.x - newColor.x;
        ey += color.y - newColor.y;
        ez += color.z - newColor.z;
        ex *= ditherlevel;
        ey *= ditherlevel;
        ez *= ditherlevel;
    }

    return index;
}

static void move(DitherImage& image, int direction)
{
    Image8Bpp& indexedImage = image.output;
    int x = image.x;
    int y = image.y;
    int width = indexedImage.width;
    int height = indexedImage.height;
    /* dither the current pixel */
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        int index = Dither(image.pixels[x + y * width], image.output.palette, image.dither, image.ditherlevel);
        indexedImage.pixels[x + y * width] = index;
    }

    /* move to the next pixel */
    switch (direction)
    {
        case LEFT:
            image.x -= 1;
            break;
        case RIGHT:
            image.x += 1;
            break;
        case UP:
            image.y -= 1;
            break;
        case DOWN:
            image.y += 1;
            break;
    }
}

void Hilbert(DitherImage& image, int level, int direction)
{
    if (level == 1)
    {
        switch (direction)
        {
            case LEFT:
                move(image, RIGHT);
                move(image, DOWN);
                move(image, LEFT);
                break;
            case RIGHT:
                move(image, LEFT);
                move(image, UP);
                move(image, RIGHT);
                break;
            case UP:
                move(image, DOWN);
                move(image, RIGHT);
                move(image, UP);
                break;
            case DOWN:
                move(image, UP);
                move(image, LEFT);
                move(image, DOWN);
                break;
        }
    }
    else
    {
        switch (direction)
        {
            case LEFT:
                Hilbert(image, level - 1, UP);
                move(image, RIGHT);
                Hilbert(image, level - 1, LEFT);
                move(image, DOWN);
                Hilbert(image, level - 1, LEFT);
                move(image, LEFT);
                Hilbert(image, level - 1, DOWN);
                break;
            case RIGHT:
                Hilbert(image, level - 1, DOWN);
                move(image, LEFT);
                Hilbert(image, level - 1, RIGHT);
                move(image, UP);
                Hilbert(image, level - 1, RIGHT);
                move(image, RIGHT);
                Hilbert(image, level - 1, UP);
                break;
            case UP:
                Hilbert(image, level - 1, LEFT);
                move(image, DOWN);
                Hilbert(image, level - 1, UP);
                move(image, RIGHT);
                Hilbert(image, level - 1, UP);
                move(image, UP);
                Hilbert(image, level - 1, RIGHT);
                break;
            case DOWN:
                Hilbert(image, level - 1, RIGHT);
                move(image, UP);
                Hilbert(image, level - 1, DOWN);
                move(image, LEFT);
                Hilbert(image, level - 1, DOWN);
                move(image, DOWN);
                Hilbert(image, level - 1, LEFT);
                break;
        }
    }
}

void RiemersmaDither(std::vector<Color>::iterator pixels, Image8Bpp& image, int dither, float ditherlevel)
{
    DitherImage dimage(pixels, image, dither, ditherlevel);
    int size = ceil(log2(std::max(image.width, image.height)));
    if (size > 0) Hilbert(dimage, size, UP);
    move(dimage, NONE);
}
