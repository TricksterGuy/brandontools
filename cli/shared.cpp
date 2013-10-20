#include "shared.hpp"
#include "fortunegen.hpp"
#include "cpercep.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <cfloat>

#ifndef CLAMP
#define CLAMP(x) (((x) < 0.0) ? 0.0 : (((x) > 31) ? 31 : (x)))
#endif

std::vector<Color> palette;
std::map<Color, int> paletteMap;
Header header;

/** Header
  *
  * Constructor
  */
Header::Header(const std::string& invocation)
{
    this->invocation = invocation;
    this->p_color = -1;
    this->mode = 3;
}

/** ~Header
  *
  * Destructor
  */
Header::~Header()
{

}

/** @brief Write
  *
  * @todo: document this function
  */
void Header::Write(std::ostream& file)
{
    char str[1024];
	time_t aclock;
	struct tm *newtime;

	time(&aclock);
	newtime = localtime( &aclock );
	strftime(str, 96, "%A %m/%d/%Y, %H:%M:%S", newtime);

    file << "/*" << std::endl;
    file << " * Exported with brandontools v" << AutoVersion::MAJOR << "." << AutoVersion::MINOR << std::endl;
    if (!invocation.empty())
        file << " * Invocation command was brandontools " << invocation << std::endl;
    file << " * Time-stamp: " << str << std::endl;
    file << " * " << std::endl;
    file << " * Image Information" << std::endl;
    file << " * -----------------" << std::endl;
    for (unsigned int i = 0; i < images.size() ; i++)
        file << " * " << images[i] << std::endl;
    if (p_color != -1)
    {
        file << " * Transparent color (";
        if (mode == 4)
        {
            Color c;
            c.x = p_color & 0x1F;
            c.y = (p_color >> 5) & 0x1F;
            c.z = (p_color >> 10) & 0x1F;
            int index = paletteSearch(c);
            c = palette[index];
            int r, g, b;
            c.Get(r, g, b);
            p_color = r | g << 5 | b << 10;
        }
        file << (p_color & 0x1F) << "," << ((p_color >> 5) & 0x1F) << "," << ((p_color >> 10) & 0x1F);
        file << ") => 0x" << std::hex << (unsigned short)p_color << std::dec << std::endl;
    }
    file << " * " << std::endl;
    file << " * Quote/Fortune of the Day!" << std::endl;
    file << " * -------------------------" << std::endl;
    file << FortuneGenerator::Instance().GetQuote();
    file << " * " << std::endl;
    file << FortuneGenerator::Instance().GetFortune();
    file << " * " << std::endl;
    file << " * All bug reports / feature requests are to be sent to Brandon (brandon.whitehead@gatech.edu)" << std::endl;
    for (unsigned int i = 0; i < lines.size() ; i++)
        file << " * " << lines[i] << std::endl;
    file << " */\n" << std::endl;
}

/** @brief AddLine
  *
  * @todo: document this function
  */
void Header::AddLine(const std::string& line)
{
    lines.push_back(line);
}

/** @brief AddImage
  *
  * @todo: document this function
  */
void Header::AddImage(const Magick::Image& image, bool frame)
{
    char buffer[1024];
    if (frame)
        sprintf(buffer, "%s (frame %d) %d@%d", image.baseFilename().c_str(), (int)image.scene(), (int)image.columns(), (int)image.rows());
    else
        sprintf(buffer, "%s %d@%d", image.baseFilename().c_str(), (int)image.columns(), (int)image.rows());
    std::string imagestr = buffer;
    images.push_back(imagestr);
}

/** @brief SetInvocation
  *
  * @todo: document this function
  */
void Header::SetInvocation(const std::string& invo)
{
    invocation = invo;
}

/** @brief SetTransparent
  *
  * @todo: document this function
  */
void Header::SetTransparent(int p_color)
{
    this->p_color = p_color;
}

/** @brief SetMode
  *
  * @todo: document this function
  */
void Header::SetMode(int mode)
{
    this->mode = mode;
}

Magick::Image ConvertToGBA(Magick::Image image)
{
    // GBA colors are 16bit ubbbbbgggggrrrrr
    // u = unused
    // CLAMP to (0, 31/255)
    // FUCK UBUNTU. QUICK HACK HERE since current libmagick package in ubuntu's repo fails.
    //image.fx("round(r*31)/255", Magick::RedChannel);
    //image.fx("round(g*31)/255", Magick::GreenChannel);
    //image.fx("round(b*31)/255", Magick::BlueChannel);

    // Ensure that there are no other references to this image.
    image.modifyImage();
    // Set the image type to TrueColor DirectClass representation.
    image.type(Magick::TrueColorType);

    unsigned char* pixels = new unsigned char[image.columns() * image.rows() * 3];
    image.write(0, 0, image.columns(), image.rows(), "RGB", Magick::CharPixel, pixels);


    int icount = 0;
    for (unsigned int i = 0; i < image.rows(); i++)
    {
        for (unsigned int j = 0; j < image.columns(); j++)
        {
            pixels[icount] = round(pixels[icount] * 31) / 255;
            pixels[icount + 1] = round(pixels[icount + 1] * 31) / 255;
            pixels[icount + 2] = round(pixels[icount + 2] * 31) / 255;
            icount += 3;
        }
    }

	std::string filename = image.baseFilename();
    image.read(image.columns(), image.rows(), "RGB", Magick::CharPixel, pixels);
	// FUCK UBUNTU x2 HACKS squared.
	image.comment(filename);

    delete[] pixels;

    return image;
}

void split(const std::string& s, char delimiter, std::vector<std::string>& tokens)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delimiter))
        tokens.push_back(item);
}

struct DitherImage
{
    DitherImage(std::vector<Color>::iterator i, std::vector<int>& outImage, int x, int y, int w, int h, int dither, float ditherlevel) : image(i), indexedImage(outImage)
    {
        this->x = x;
        this->y = y;
        this->width = w;
        this->height = h;
        this->dither = dither;
        this->ditherlevel = ditherlevel;
    }
    std::vector<Color>::iterator image;
    std::vector<int>& indexedImage;
    int width, height;
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

int paletteSearch(Color a)
{
    register double bestd = DBL_MAX;
    int index = -1;

    if (paletteMap.find(a) != paletteMap.end())
        return paletteMap[a];

    for (unsigned int i = 0; i < palette.size(); i++)
    {
        double dist = 0;
        Color b = palette[i];

        dist = a.Distance(b);
        if (dist < bestd)
        {
            index = i;
            bestd = dist;
        }
    }

    paletteMap[a] = index;

    return index;
}

int Dither(Color& color, int dither, float ditherlevel)
{
    static int ex = 0, ey = 0, ez = 0;
    Color newColor(CLAMP(color.x + ex), CLAMP(color.y + ey), CLAMP(color.z + ez));
    int index = paletteSearch(newColor);
    newColor = palette[index];

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
    /* dither the current pixel */
    if (image.x >= 0 && image.x < image.width && image.y >= 0 && image.y < image.height)
    {
        int index = Dither(image.image[image.x + image.y * image.width], image.dither, image.ditherlevel);
        image.indexedImage[image.x + image.y * image.width] = index;
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

void RiemersmaDither(std::vector<Color>::iterator image, std::vector<int>& indexedImage, int width, int height, int dither, float ditherlevel)
{
    DitherImage dimage(image, indexedImage, 0, 0, width, height, dither, ditherlevel);
    int size = std::max(width, height);
    size = ceil(log2(size));
    if (size > 0) Hilbert(dimage, size, UP);
    move(dimage, NONE);
}

void Chop(std::string& filename)
{
    int index = filename.rfind('\\');
    if ((unsigned int)index == std::string::npos)
    {
        index = filename.rfind('/');
        if ((unsigned int)index == std::string::npos) index = -1;
    }

    filename = filename.substr(index+1);
}

std::string Sanitize(const std::string& filename)
{
    std::stringstream out;
    for (int i = 0; i < filename.size(); i++)
    {
        if ((filename[i] >= 'A' && filename[i] <= 'Z') || (filename[i] >= 'a' && filename[i] <= 'z') || (filename[i] >= '0' && filename[i] <= '9') || filename[i] == '_')
            out.put(filename[i]);
    }
    return out.str();
}
