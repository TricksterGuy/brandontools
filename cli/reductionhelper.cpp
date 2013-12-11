#include "reductionhelper.hpp"
#include <cfloat>

using namespace std;

std::vector<Color> palette;
std::map<Color, int> paletteMap;

Magick::Image ConvertToGBA(Magick::Image image)
{
    // GBA colors are 16bit ubbbbbgggggrrrrr
    // u = unused
    // CLAMP to (0, 31/255)
    // QUICK HACK HERE since current libmagick package in ubuntu's repo fails.
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
	image.comment(filename);

    delete[] pixels;

    return image;
}

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

void QuantizeImage(Magick::Image image, const ExportParams& params, IndexedImage& indexedImage)
{
    unsigned int num_pixels = image.rows() * image.columns();
    // If the image width is odd warn them
    if (image.columns() % 2)
        printf("[WARNING] Image width is not a multiple of 2\n");

    image = ConvertToGBA(image);

    unsigned char* imagePixels = new unsigned char[num_pixels * 3];
    std::vector<Color> pixels(num_pixels);
    palette.clear();
    image.write(0, 0, image.columns(), image.rows(), "RGB", Magick::CharPixel, imagePixels);

    for (unsigned int i = 0; i < num_pixels; i++)
        pixels[i].Set(imagePixels[3 * i + 0], imagePixels[3 * i + 1], imagePixels[3 * i + 2]);
    delete[] imagePixels;

    MedianCut(pixels, params.palette, palette, params.weights);
    RiemersmaDither(pixels.begin(), indexedImage, image.columns(), image.rows(), params.dither, params.dither_level);
}

struct DitherImage
{
    DitherImage(std::vector<Color>::iterator i, IndexedImage& outImage,
                int x, int y, int w, int h, int dither, float ditherlevel) : image(i), indexedImage(outImage)
    {
        this->x = x;
        this->y = y;
        this->width = w;
        this->height = h;
        this->dither = dither;
        this->ditherlevel = ditherlevel;
    }
    std::vector<Color>::iterator image;
    IndexedImage& indexedImage;
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

void RiemersmaDither(std::vector<Color>::iterator image, IndexedImage& indexedImage, int width,
                     int height, int dither, float ditherlevel)
{
    DitherImage dimage(image, indexedImage, 0, 0, width, height, dither, ditherlevel);
    int size = ceil(log2(std::max(width, height)));
    if (size > 0) Hilbert(dimage, size, UP);
    move(dimage, NONE);
}

void FormPaletteAndIndexedImages(std::vector<Magick::Image> images, const ExportParams& params, std::vector<IndexedImage>& indexedImages)
{
    for (unsigned int i = 0; i < images.size(); i++)
        images[i] = ConvertToGBA(images[i]);

    // TODO make this more memory efficient.
    unsigned int total_pixels = 0;
    for (unsigned int i = 0; i < images.size(); i++)
        total_pixels += images[i].rows() * images[i].columns();
    unsigned char* all_pixels = new unsigned char[total_pixels * 3];
    unsigned char* current = all_pixels;

    for (unsigned int i = 0; i < images.size(); i++)
    {
        images[i].write(0, 0, images[i].columns(), images[i].rows(), "RGB", Magick::CharPixel, current);
        current += images[i].columns() * images[i].rows() * 3;
    }
    std::vector<Color> pixels(total_pixels);
    for (unsigned int i = 0; i < total_pixels; i++)
        pixels[i].Set(all_pixels[3 * i + 0], all_pixels[3 * i + 1], all_pixels[3 * i + 2]);
    delete[] all_pixels;
    MedianCut(pixels, params.palette, palette, params.weights);

    indexedImages.resize(images.size());

    // Dither and form all images
    int location = 0;
    for (unsigned int i = 0; i < images.size(); i++)
    {
        indexedImages[i].resize(images[i].rows() * images[i].columns(), 1);
        RiemersmaDither(pixels.begin() + location, indexedImages[i], images[i].columns(), images[i].rows(), params.dither, params.dither_level);
        location += images[i].columns() * images[i].rows();
    }
}

void GetTiles8bpp(std::vector<Magick::Image> images, const ExportParams& params, std::vector<Tile>& tiles, std::vector<short>& offsets)
{
    int tile_width = 8 + params.border;
    std::vector<IndexedImage> indexedImages;
    FormPaletteAndIndexedImages(images, params, indexedImages);

    std::set<Tile> tileSet;

    tiles.push_back(NULLTILE);
    tileSet.insert(NULLTILE);

    for (unsigned int k = 0; k < images.size(); k++)
    {
        IndexedImage& indexedImage = indexedImages[k];
        Magick::Image image = images[k];

        offsets.push_back(tiles.size());
        unsigned int tilesX = image.columns() / tile_width;
        unsigned int tilesY = image.rows() / tile_width;
        unsigned int totalTiles = tilesX * tilesY;
        // Perform reduce.
        for (unsigned int i = 0; i < totalTiles; i++)
        {
            int tilex = i % tilesX;
            int tiley = i / tilesX;
            Tile tile(indexedImage, image.columns(), tilex, tiley, params.border);
            std::set<Tile>::iterator foundTile = tileSet.find(tile);
            if (foundTile == tileSet.end())
            {
                tile.id = tiles.size();
                tiles.push_back(tile);
                tileSet.insert(tile);
            }
        }
    }

    // Checks
    int tile_size = params.bpp == 8 ? TILE_SIZE_BYTES_8BPP : TILE_SIZE_BYTES_4BPP;
    int memory_b = tiles.size() * tile_size;
    if (tiles.size() >= 1024)
    {
        printf("[ERROR] Too many tiles found %zd tiles.\n"
               "Please use -reduce or make the image simpler.\n", tiles.size());
        exit(EXIT_FAILURE);
    }

    // Delicious infos
    int cbbs = tiles.size() * tile_size / SIZE_CBB_BYTES;
    int sbbs = (int) ceil(tiles.size() * tile_size % SIZE_CBB_BYTES / ((double)SIZE_SBB_BYTES));
    printf("[INFO] Tiles found %zu.\n", tiles.size());
    printf("[INFO] Tiles uses %d charblocks and %d screenblocks.\n", cbbs, sbbs);
    printf("[INFO] Total utilization %.2f/4 charblocks or %d/32 screenblocks, %d/65536 bytes.\n",
           memory_b / ((double)SIZE_CBB_BYTES), (int) ceil(memory_b / ((double)SIZE_SBB_BYTES)), memory_b);

}

unsigned short PackPixels(const void* pixelArray, unsigned int i, const void* unused)
{
    // pixels[3*i] = r, pixels[3*i+1] = g, pixels[3*i+2] = b
    const char* pixels = static_cast<const char*>(pixelArray);
    return pixels[3 * i] | (pixels[3 * i + 1] << 5) | (pixels[3 * i + 2] << 10);
}

unsigned short GetPaletteEntry(const void* paletteArray, unsigned int i, const void* offset_ptr)
{
    unsigned int p_offset = *static_cast<const unsigned int*>(offset_ptr);
    // Return a dummy color (black) for the first offset colors.
    if (i < p_offset)
        return 0x8000;

    const Color* palette = static_cast<const Color*>(paletteArray);
    int x, y, z;
    const Color& color = palette[i - p_offset];
    color.Get(x, y, z);
    return x | (y << 5) | (z << 10);
}

unsigned short GetIndexedEntry(const void* indicesArray, unsigned int i, const void* offset_ptr)
{
    unsigned int p_offset = *static_cast<const unsigned int*>(offset_ptr);
    const unsigned char* indices = static_cast<const unsigned char*>(indicesArray);
    int px1 = indices[2 * i] + p_offset;
    int px2 = indices[2 * i + 1] + p_offset;
    return px1 | (px2 << 8);
}

std::string getAnimFrameName(const void* stringArray, unsigned int i, const void* unused)
{
    const std::string* strings = static_cast<const std::string*>(stringArray);
    return strings[i];
}

void WriteTiles(ostream& file, const ExportParams& params, const std::string& name, const std::vector<Tile>& tiles)
{
    file << "const unsigned short " << name << "_tiles[" << tiles.size() * (params.bpp == 8 ? TILE_SIZE_SHORTS_8BPP : TILE_SIZE_BYTES_4BPP) << "] =\n{\n\t";
    for (unsigned int i = 0; i < tiles.size(); i++)
    {
        file << tiles[i];
        if (i != tiles.size() - 1)
            file << ",\n\t";
    }
    file << "\n};\n";
}

