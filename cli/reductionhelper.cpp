#include "reductionhelper.hpp"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <sstream>

#include "cpercep.hpp"
#include "dither.hpp"
#include "fileutils.hpp"
#include "mediancut.hpp"
#include "shared.hpp"

const int sprite_shapes[16] =
{
//h =  1,  2, 4,  8
    0,  2, 2, -1, // width = 1
    1,  0, 2, -1, // width = 2
    1,  1, 0,  2, // width = 4
    -1, -1, 1,  0  // width = 8
};

const int sprite_sizes[16] =
{
//h =  1,  2, 4,  8
    0,  0, 1, -1, // width = 1
    0,  1, 2, -1, // width = 2
    1,  2, 2,  3, // width = 4
    -1, -1, 3,  3  // width = 8
};


Image16Bpp::Image16Bpp(Magick::Image image, const std::string& _name) : width(image.columns()), height(image.rows()), name(_name), pixels(width * height)
{
    unsigned int num_pixels = width * height;
    const Magick::PixelPacket* imageData = image.getConstPixels(0, 0, image.columns(), image.rows());

    size_t depth;
    MagickCore::GetMagickQuantumDepth(&depth);
    for (unsigned int i = 0; i < num_pixels; i++)
    {
        const Magick::PixelPacket& packet = imageData[i];
        int r, g, b;
        if (depth == 8)
        {
            r = (packet.red >> 3) & 0x1F;
            g = (packet.green >> 3) & 0x1F;
            b = (packet.blue >> 3) & 0x1F;
        }
        else if (depth == 16)
        {
            r = (packet.red >> 11) & 0x1F;
            g = (packet.green >> 11) & 0x1F;
            b = (packet.blue >> 11) & 0x1F;
        }
        else
        {
            throw "[FATAL] Image quantum not supported\n";
        }
        pixels[i] = r | g << 5 | b << 10;
    }
}

void Image16Bpp::GetColors(std::vector<Color>& colors) const
{
    for (unsigned int i = 0; i < width * height; i++)
    {
        short pix = pixels[i];
        if (pix != params.transparent_color)
        {
            colors.push_back(Color(pix));
        }
    }
}

Image16Bpp Image16Bpp::SubImage(unsigned int x, unsigned int y, unsigned int swidth, unsigned int sheight) const
{
    Image16Bpp sub(swidth, sheight);
    for (unsigned int i = 0; i < sheight; i++)
    {
        for (unsigned int j = 0; j < swidth; j++)
        {
            sub.pixels[i * swidth + j] = pixels[(i + y) * width + j + x];
        }
    }
    return sub;
}

void Image16Bpp::WriteData(std::ostream& file) const
{
    WriteShortArray(file, name, "", pixels, 10);
    WriteNewLine(file);
}

void Image16Bpp::WriteExport(std::ostream& file) const
{
    WriteExternShortArray(file, name, "", pixels.size());
    WriteDefine(file, name, "_SIZE", pixels.size());
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
    WriteNewLine(file);
}

Palette::Palette(const std::vector<Color>& _colors, const std::string& _name) : colors(_colors), name(_name)
{
    if (colors.size() + params.offset > 256)
    {
        std::stringstream oss;
        oss << "[ERROR] Too many colors in palette.\nFound " << (colors.size() + params.offset) << " colors, offset is " << params.offset << ".\n";
        throw oss.str();
    }
};

void Palette::Set(const std::vector<Color>& _colors)
{
    colors = _colors;
}

int Palette::Search(const Color& a) const
{
    register double bestd = DBL_MAX;
    int index = -1;

    if (colorIndexCache.find(a) != colorIndexCache.end())
        return colorIndexCache[a];

    for (unsigned int i = 0; i < colors.size(); i++)
    {
        double dist = 0;
        const Color& b = colors[i];
        dist = a.Distance(b);
        if (dist < bestd)
        {
            index = i;
            bestd = dist;
        }
    }

    colorIndexCache[a] = index;

    return index;
}

int Palette::Search(unsigned short color_data) const
{
    return Search(Color(color_data));
}

void Palette::WriteData(std::ostream& file) const
{
    WriteShortArray(file, name, "_palette", colors, 10);
    WriteNewLine(file);
}

void Palette::WriteExport(std::ostream& file) const
{
    WriteExternShortArray(file, name, "_palette", colors.size());
    WriteDefine(file, name, "_PALETTE_SIZE", colors.size());
    WriteNewLine(file);
}

Image8Bpp::Image8Bpp(const Image16Bpp& image) : width(image.width), height(image.height), name(image.name), pixels(width * height), palette(NULL)
{
    // If the image width is odd error out
    if (width & 1)
    {
        std::stringstream oss;
        oss << "[ERROR] Image" << name << " width is not a multiple of 2. Please fix\n";
        throw oss.str();
    }

    std::vector<Color> pixels16;
    image.GetColors(pixels16);

    // If transparent color is present add to palette, but only if offset is not 0
    std::vector<Color> paletteColors;
    paletteColors.reserve(params.palette);
    if (params.offset == 0)
    {
        params.palette -= 1;
        paletteColors.push_back(Color(params.transparent_color));
    }

    MedianCut(pixels16, params.palette, paletteColors, params.weights);

    palette.reset(new Palette(paletteColors, name));
    RiemersmaDither(image, *this, params.transparent_color, params.dither, params.dither_level);
    if (params.offset > 0)
    {
        for (unsigned char& pix : pixels)
            if (pix)
                pix += params.offset;
    }
}

void Image8Bpp::Set(const Image16Bpp& image, std::shared_ptr<Palette> global_palette)
{
    width = image.width;
    height = image.height;
    name = image.name;
    pixels.resize(width * height);
    palette = global_palette;

    if (width & 1)
    {
        std::stringstream oss;
        oss << "[ERROR] Image" << name << " width is not a multiple of 2. Please fix\n";
        throw oss.str();
    }

    RiemersmaDither(image, *this, params.transparent_color, params.dither, params.dither_level);
    if (params.offset > 0)
    {
        for (unsigned char& pix : pixels)
            if (pix)
                pix += params.offset;
    }
}

void Image8Bpp::WriteData(std::ostream& file) const
{
    WriteShortArray(file, name, "", pixels, 10);
    WriteNewLine(file);
}

void Image8Bpp::WriteExport(std::ostream& file) const
{
    WriteExternShortArray(file, name, "", pixels.size()/2);
    WriteDefine(file, name, "_SIZE", pixels.size()/2);
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
    WriteNewLine(file);
}

Image8BppScene::Image8BppScene(const std::vector<Image16Bpp>& images16, const std::string& _name) : name(_name), images(images16.size()), palette(NULL)
{
    unsigned int total_pixels = 0;
    for (unsigned int i = 0; i < images16.size(); i++)
        total_pixels += images16[i].width * images16[i].height;

    std::vector<Color> pixels;
    pixels.reserve(total_pixels);
    for (unsigned int i = 0; i < images16.size(); i++)
        images16[i].GetColors(pixels);

    // If transparent color is present add to palette
    std::vector<Color> paletteColors;
    paletteColors.reserve(params.palette);
    if (params.offset == 0)
    {
        params.palette -= 1;
        paletteColors.push_back(Color(params.transparent_color));
    }

    MedianCut(pixels, params.palette, paletteColors, params.weights);
    palette.reset(new Palette(paletteColors, name));

    for (unsigned int i = 0; i < images.size(); i++)
        images[i].Set(images16[i], palette);
}

void Image8BppScene::WriteData(std::ostream& file) const
{
    palette->WriteData(file);
    for (const auto& image : images)
        image.WriteData(file);
}

void Image8BppScene::WriteExport(std::ostream& file) const
{
    palette->WriteExport(file);
    for (const auto& image : images)
        image.WriteExport(file);
}

PaletteBank::PaletteBank(const std::vector<Color>& _colors, int _id) : colors(_colors), id(_id)
{
    colorSet.insert(colors.begin(), colors.end());
}

bool PaletteBank::Contains(const Palette& palette) const
{
    for (const auto& color : palette.colors)
    {
        if (colorSet.find(color) == colorSet.end())
            return false;
    }
    return true;
}

int PaletteBank::CanMerge(const Palette& palette) const
{
    std::set<Color> tmpset(colorSet);
    for (const auto& color : palette.colors)
        tmpset.insert(color);

    return 16 - tmpset.size();
}

void PaletteBank::Merge(const Palette& palette)
{
    for (const auto& color : palette.colors)
    {
        if (colorSet.find(color) == colorSet.end())
        {
            colorSet.insert(color);
            colors.push_back(color);
        }
    }
}

void PaletteBank::Add(const Color& c)
{
    if (colorSet.find(c) == colorSet.end())
    {
        colorSet.insert(c);
        colors.push_back(c);
    }
}

int PaletteBank::Search(const Color& a) const
{
    register double bestd = DBL_MAX;
    int index = -1;

    for (unsigned int i = 0; i < colors.size(); i++)
    {
        double dist = 0;
        const Color& b = colors[i];
        dist = a.Distance(b);
        if (dist < bestd)
        {
            index = i;
            bestd = dist;
        }
    }

    if (bestd != 0)
    {
        fprintf(stderr, "[WARNING] Color remap: Color given to palette bank not an exact match.\n");
    }

    return index;
}

std::ostream& operator<<(std::ostream& file, const PaletteBank& bank)
{
    std::vector<Color> colors = bank.colors;
    colors.resize(16);
    for (unsigned int i = 0; i < colors.size(); i++)
    {
        int x, y, z;
        const Color& color = colors[i];
        color.Get(x, y, z);
        short data_read = x | (y << 5) | (z << 10);
        WriteElement(file, data_read, colors.size(), i, 8);
    }

    return file;
}

void WriteExportPaletteBanks(std::ostream& file, const std::string& name, const std::vector<PaletteBank>& banks)
{
    WriteExternShortArray(file, name, "_palette", 256);
    WriteDefine(file, name, "_PALETTE_SIZE", 256);
    WriteNewLine(file);
}

void WriteDataPaletteBanks(std::ostream& file, const std::string& name, const std::vector<PaletteBank>& banks)
{
    file << "const unsigned short " << name << "_palette[256] =\n{\n\t";
    for (unsigned int i = 0; i < 16; i++)
    {
        file << banks[i];
        if (i != 16 - 1)
            file << ",\n\t";
    }
    file << "\n};\n";
    WriteNewLine(file);
}

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
Tile<unsigned char>::Tile(std::shared_ptr<ImageTile> imageTile, int _bpp) : data(TILE_SIZE), bpp(_bpp)
{
    // bpp reduce minus one for the transparent color
    int num_colors = (1 << bpp) - 1;
    const unsigned short* imgdata = imageTile->data.data();
    int weights[4] = {25, 25, 25, 25};

    std::vector<Color> pixels;
    pixels.reserve(TILE_SIZE);

    std::vector<Color> paletteArray;
    paletteArray.reserve(1 << bpp);

    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        unsigned short pix = imgdata[i];
        if (pix != params.transparent_color)
            pixels.push_back(Color(pix));
    }
    paletteArray.push_back(Color(params.transparent_color));
    MedianCut(pixels, num_colors, paletteArray, weights);

    palette.Set(paletteArray);

    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        unsigned short pix = imgdata[i];
        data[i] = (pix != params.transparent_color) ? palette.Search(pix) : 0;
    }

    sourceTile = imageTile;
}

template <>
void Tile<unsigned char>::Set(const Image16Bpp& image, const Palette& global_palette, int tilex, int tiley)
{
    bpp = 8;
    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        int x = i % 8;
        int y = i / 8;
        data[i] = global_palette.Search(image.pixels[(tiley * 8 + y) * image.width + tilex * 8 + x]);
    }
}

template <>
void Tile<unsigned char>::UsePalette(const PaletteBank& bank)
{
    if (bpp == 8)
    {
        fprintf(stderr, "Not implemented");
        return;
    }

    std::map<int, int> remapping;
    for (unsigned int i = 0; i < palette.Size(); i++)
    {
        Color old = palette.At(i);
        remapping[i] = bank.Search(old);
    }

    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        if (remapping.find(data[i]) == remapping.end())
            throw "[FATAL] Somehow tile contains invalid indicies. This shouldn't happen";

        data[i] = remapping[data[i]];
    }

    palette.Set(bank.colors);
}

template <>
std::ostream& operator<<(std::ostream& file, const Tile<unsigned char>& tile)
{
    const std::vector<unsigned char>& data = tile.data;
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
    return i.palette.Size() > j.palette.Size();
}

Tileset::Tileset(const std::vector<Image16Bpp>& images, const std::string& _name, int _bpp) : name(_name), bpp(_bpp)
{
    switch(bpp)
    {
    case 4:
        Init4bpp(images);
        break;
    case 8:
        Init8bpp(images);
        break;
    case 16:
        Init16bpp(images);
        break;
    }
}

Tileset::Tileset(const Image16Bpp& image, int _bpp) : name(image.name), bpp(_bpp)
{
    std::vector<Image16Bpp> images;
    images.push_back(image);
    switch(bpp)
    {
    case 4:
        Init4bpp(images);
        break;
    case 8:
        Init8bpp(images);
        break;
    case 16:
        Init16bpp(images);
        break;
    }
}

int Tileset::Search(const GBATile& tile) const
{
    const std::set<GBATile>::const_iterator foundTile = tiles.find(tile);
    if (foundTile != tiles.end())
    {
        std::cout << "found\n" << *foundTile << "\n";
        return foundTile->id;
    }

    return -1;
}

int Tileset::Search(const ImageTile& tile) const
{
    const std::set<ImageTile>::const_iterator foundTile = itiles.find(tile);
    if (foundTile != itiles.end())
        return foundTile->id;

    return -1;
}

bool Tileset::Match(const ImageTile& tile, int& tile_id, int& pal_id) const
{
    const std::map<ImageTile, GBATile>::const_iterator foundTile = matcher.find(tile);
    if (foundTile != matcher.end())
    {
        const GBATile& tile = foundTile->second;
        tile_id = tile.id;
        pal_id = tile.palette_bank;
        return true;
    }

    return false;
}

void Tileset::WriteData(std::ostream& file) const
{
    if (bpp == 8)
        palette->WriteData(file);
    else
        WriteDataPaletteBanks(file, name, paletteBanks);

    std::vector<GBATile>::const_iterator tile_ptr = tilesExport.begin();
    file << "const unsigned short " << name << "_tiles[" << Size() << "] =\n{\n\t";
    for (unsigned int i = 0; i < tilesExport.size(); i++)
    {
        file << *tile_ptr;
        if (i != tilesExport.size() - 1)
            file << ",\n\t";
        tile_ptr++;
    }
    file << "\n};\n";
    WriteNewLine(file);
}

void Tileset::WriteExport(std::ostream& file) const
{
    if (bpp == 8)
        palette->WriteExport(file);
    else
        WriteExportPaletteBanks(file, name, paletteBanks);

    WriteDefine(file, name, "_PALETTE_TYPE", (bpp == 8), 7);
    WriteNewLine(file);

    WriteExternShortArray(file, name, "_tiles", Size());
    WriteDefine(file, name, "_TILES", tiles.size());
    WriteDefine(file, name, "_TILES_SIZE", Size());
    WriteNewLine(file);
}

void Tileset::Init4bpp(const std::vector<Image16Bpp>& images)
{
    // Initialize Palette Banks
    paletteBanks.reserve(16);
    for (int i = 0; i < 16; i++)
        paletteBanks.push_back(PaletteBank(i));

    // Tile image into 16 bit tiles
    Tileset tileset16bpp(images, name, 16);
    std::set<ImageTile> imageTiles = tileset16bpp.itiles;

    GBATile nullTile;
    nullTile.id = 0;
    nullTile.bpp = 4;
    ImageTile nullImageTile;
    tiles.insert(nullTile);
    tilesExport.push_back(nullTile);
    matcher[nullImageTile] = nullTile;

    // Reduce each tile to 4bpp
    std::vector<GBATile> gbaTiles;
    for (std::set<ImageTile>::const_iterator i = imageTiles.begin(); i != imageTiles.end(); ++i)
    {
        std::shared_ptr<ImageTile> src(new ImageTile(*i));
        GBATile tile(src, 4);
        gbaTiles.push_back(tile);
    }

    // Ensure image contains < 256 colors
    std::set<Color> bigPalette;
    for (const auto& tile : gbaTiles)
    {
        const std::vector<Color>& tile_palette = tile.palette.colors;
        bigPalette.insert(tile_palette.begin(), tile_palette.end());
    }

    if (bigPalette.size() > 256 && !params.force)
    {
        std::stringstream oss;
        oss << "[ERROR] Image after reducing tiles to 4bpp still contains more than 256 distinct colors.  Found " << bigPalette.size() << " colors. Please fix.";
        throw oss.str();
    }

    // Greedy approach deal with tiles with largest palettes first.
    std::sort(gbaTiles.begin(), gbaTiles.end(), TilesPaletteSizeComp);

    // But deal with nulltile
    for (unsigned int i = 0; i < paletteBanks.size(); i++)
        paletteBanks[i].Add(Color());
    nullTile.palette_bank = 0;


    // Construct palette banks, assign bank id to tile, remap tile to palette bank given, assign tile ids
    for (auto& tile : gbaTiles)
    {
        int pbank = -1;
        // Fully contains checks
        for (unsigned int i = 0; i < paletteBanks.size(); i++)
        {
            PaletteBank& bank = paletteBanks[i];
            if (bank.Contains(tile.palette))
                pbank = i;
        }

        // Ok then find least affected bank
        if (pbank == -1)
        {
            int max_colors_left = -1;
            for (unsigned int i = 0; i < paletteBanks.size(); i++)
            {
                PaletteBank& bank = paletteBanks[i];
                int colors_left = bank.CanMerge(tile.palette);
                if (colors_left != 0 && max_colors_left < colors_left)
                {
                    max_colors_left = colors_left;
                    pbank = i;
                }
            }
        }

        // Cry and die for now. Unless you tell me to keep going.
        if (pbank == -1 && !params.force)
            throw "More than 16 distinct palettes found, please use 8bpp mode.";

        // Merge step and assign palette bank
        paletteBanks[pbank].Merge(tile.palette);
        tile.palette_bank = pbank;
        tile.UsePalette(paletteBanks[pbank]);

        // Assign tile id
        std::set<GBATile>::const_iterator it = tiles.find(tile);
        if (it == tiles.end())
        {
            tile.id = tiles.size();
            tiles.insert(tile);
            tilesExport.push_back(tile);
        }
        else
        {
            tile.id = it->id;
        }

        // Form mapping from ImageTile to GBATile
        matcher[*tile.sourceTile] = tile;
    }

    int tile_size = TILE_SIZE_BYTES_4BPP;
    int memory_b = tiles.size() * tile_size;
    if (tiles.size() >= 1024)
    {
        std::stringstream oss;
        oss << "[ERROR] Too many tiles found" << tiles.size() << "tiles.\nPlease make the image simpler.\n";
        throw oss.str();
    }

    // Delicious infos
    int cbbs = tiles.size() * tile_size / SIZE_CBB_BYTES;
    int sbbs = (int) ceil(tiles.size() * tile_size % SIZE_CBB_BYTES / ((double)SIZE_SBB_BYTES));
    fprintf(stderr, "[INFO] Tiles found %zu.\n", tiles.size());
    fprintf(stderr, "[INFO] Tiles uses %d charblocks and %d screenblocks.\n", cbbs, sbbs);
    fprintf(stderr, "[INFO] Total utilization %.2f/4 charblocks or %d/32 screenblocks, %d/65536 bytes.\n",
           memory_b / ((double)SIZE_CBB_BYTES), (int) ceil(memory_b / ((double)SIZE_SBB_BYTES)), memory_b);
}

void Tileset::Init8bpp(const std::vector<Image16Bpp>& images16)
{
    int tile_width = 8 + params.border;

    // Reduce all and get the global palette and reduced images.
    Image8BppScene scene(images16, name);
    palette = scene.palette;

    const std::vector<Image8Bpp>& images = scene.images;

    GBATile nullTile;
    ImageTile nullImageTile;
    tiles.insert(nullTile);
    tilesExport.push_back(nullTile);
    matcher[nullImageTile] = nullTile;

    for (unsigned int k = 0; k < images.size(); k++)
    {
        bool disjoint_error = false;
        const Image8Bpp& image = images[k];
        const Image16Bpp& image16 = images16[k];

        offsets.push_back(tiles.size());
        unsigned int tilesX = image.width / tile_width;
        unsigned int tilesY = image.height / tile_width;
        unsigned int totalTiles = tilesX * tilesY;

        // Perform reduce.
        for (unsigned int i = 0; i < totalTiles; i++)
        {
            int tilex = i % tilesX;
            int tiley = i / tilesX;
            GBATile tile(image.pixels, image.width, tilex, tiley, params.border);
            std::set<GBATile>::iterator foundTile = tiles.find(tile);
            if (foundTile == tiles.end())
            {
                tile.id = tiles.size();
                tiles.insert(tile);
                tilesExport.push_back(tile);
                // Add matcher data
                ImageTile imageTile(image16.pixels, image.width, tilex, tiley, params.border);
                matcher[imageTile] = tile;
            }
            else if (offsets.size() > 1 && !disjoint_error)
            {
                fprintf(stderr, "[WARNING] Tiles found in tileset image %s are not disjoint, offset calculations may be off\n", image.name.c_str());
                disjoint_error = true;
            }
        }
    }

    // Checks
    int tile_size = TILE_SIZE_BYTES_8BPP;
    int memory_b = tiles.size() * tile_size;
    if (tiles.size() >= 1024)
    {
        std::stringstream oss;
        oss << "[ERROR] Too many tiles found" << tiles.size() << "tiles.\nPlease make the image simpler.\n";
        throw oss.str();
    }

    // Delicious infos
    int cbbs = tiles.size() * tile_size / SIZE_CBB_BYTES;
    int sbbs = (int) ceil(tiles.size() * tile_size % SIZE_CBB_BYTES / ((double)SIZE_SBB_BYTES));
    fprintf(stderr, "[INFO] Tiles found %zu.\n", tiles.size());
    fprintf(stderr, "[INFO] Tiles uses %d charblocks and %d screenblocks.\n", cbbs, sbbs);
    fprintf(stderr, "[INFO] Total utilization %.2f/4 charblocks or %d/32 screenblocks, %d/65536 bytes.\n",
           memory_b / ((double)SIZE_CBB_BYTES), (int) ceil(memory_b / ((double)SIZE_SBB_BYTES)), memory_b);
}

void Tileset::Init16bpp(const std::vector<Image16Bpp>& images)
{
    int tile_width = 8 + params.border;
    ImageTile nullTile;
    itiles.insert(nullTile);

    for (unsigned int k = 0; k < images.size(); k++)
    {
        const Image16Bpp& image = images[k];
        const std::vector<unsigned short>& pixels = image.pixels;

        unsigned int tilesX = image.width / tile_width;
        unsigned int tilesY = image.height / tile_width;
        unsigned int totalTiles = tilesX * tilesY;

        // Perform reduce.
        for (unsigned int i = 0; i < totalTiles; i++)
        {
            int tilex = i % tilesX;
            int tiley = i / tilesX;
            ImageTile tile(pixels, image.width, tilex, tiley, params.border);
            std::set<ImageTile>::iterator foundTile = itiles.find(tile);
            if (foundTile == itiles.end())
            {
                tile.id = itiles.size();
                itiles.insert(tile);
            }
        }
    }
}

Map::Map(const Image16Bpp& image, int bpp) : width(image.width / 8), height(image.height / 8), name(image.name), data(width * height), tileset(NULL)
{
    // Create tileset according to bpp
    tileset.reset(new Tileset(image, bpp));

    // Tile match each tile in image
    switch(bpp)
    {
    case 4:
        Init4bpp(image);
        break;
    default:
        Init8bpp(image);
        break;
    }
}

Map::Map(const Image16Bpp& image, std::shared_ptr<Tileset> global_tileset)
{
    Set(image, global_tileset);
}

void Map::Set(const Image16Bpp& image, std::shared_ptr<Tileset> global_tileset)
{
    width = image.width / 8;
    height = image.height / 8;
    name = image.name;
    data.resize(width * height);
    tileset = global_tileset;

    switch(tileset->bpp)
    {
    case 4:
        Init4bpp(image);
        break;
    default:
        Init8bpp(image);
        break;
    }
}

void Map::Init4bpp(const Image16Bpp& image)
{
    const std::vector<unsigned short>& pixels = image.pixels;

    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;
        ImageTile imageTile(pixels, image.width, tilex, tiley);
        int tile_id = 0;
        int pal_id = 0;

        if (!tileset->Match(imageTile, tile_id, pal_id))
        {
            fprintf(stderr, "[WARNING] Image: %s No match for tile starting at (%d %d) px, using empty tile instead.\n", image.name.c_str(), tilex * 8, tiley * 8);
            fprintf(stderr, "[WARNING] Image: %s No match for palette for tile starting at (%d %d) px, using palette 0 instead.\n", image.name.c_str(), tilex * 8, tiley * 8);
        }
        data[i] = pal_id << 12 | tile_id;

    }
}

void Map::Init8bpp(const Image16Bpp& image)
{
    const std::vector<unsigned short>& pixels = image.pixels;

    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;
        ImageTile tile(pixels, image.width, tilex, tiley);
        int tile_id = 0;
        int pal_id = 0;

        if (!tileset->Match(tile, tile_id, pal_id))
        {
            fprintf(stderr, "[WARNING] Image: %s No match for tile starting at (%d %d) px, using empty tile instead.\n", image.name.c_str(), tilex * 8, tiley * 8);
        }

        data[i] = tile_id;
    }
}

void Map::WriteData(std::ostream& file) const
{
    int type = (width > 32 ? 1 : 0) | (height > 32 ? 1 : 0) << 1;
    int num_blocks = (type == 0 ? 1 : (type < 3 ? 2 : 4));

    file << "const unsigned short " << name << "_map[" << num_blocks * 32 * 32 << "] =\n{\n\t";
    for (int i = 0; i < num_blocks; i++)
    {
        // Case for each possible value of num_blocks
        // 1: 0
        // 2: type is 1 - 0, 1
        //    type is 2 - 0, 2
        // 4: 0, 1, 2, 3
        int sbb = (i == 0 ? 0 : (i == 1 && type == 2 ? 2 : i));
        unsigned int sx, sy;
        sx = ((sbb & 1) != 0) * 32;
        sy = ((sbb & 2) != 0) * 32;
        for (unsigned int y = 0; y < 32; y++)
        {
            for (unsigned int x = 0; x < 32; x++)
            {
                // Read tile if outside bounds replace with null tile
                unsigned short tile_id;
                if (x + sx > width || y + sy > height)
                    tile_id = 0;
                else
                    tile_id = data[(y + sy) * width + (x + sx)];
                // Write it.
                WriteElement(file, tile_id, num_blocks * 32 * 32, (y + sy) * width + (x + sx), 8);
            }
        }
    }
    file << "\n};\n";
    WriteNewLine(file);
}

void Map::WriteExport(std::ostream& file) const
{
    WriteExternShortArray(file, name, "_map", Size());
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
    WriteDefine(file, name, "_MAP_SIZE", Size());
    WriteDefine(file, name, "_MAP_TYPE", Type(), 14);
    WriteNewLine(file);
}

MapScene::MapScene(const std::vector<Image16Bpp>& images, const std::string& _name, int bpp) : name(_name), maps(images.size()), tileset(NULL)
{
    tileset.reset(new Tileset(images, name, bpp));

    for (unsigned int i = 0; i < maps.size(); i++)
    {
        maps[i].Set(images[i], tileset);
    }
}

MapScene::MapScene(const std::vector<Image16Bpp>& images, const std::string& _name, std::shared_ptr<Tileset> _tileset) : name(_name), maps(images.size()), tileset(_tileset)
{
    for (unsigned int i = 0; i < maps.size(); i++)
    {
        maps[i].Set(images[i], tileset);
    }
}

void MapScene::WriteData(std::ostream& file) const
{
    tileset->WriteData(file);
    for (const auto& map : maps)
        map.WriteData(file);
}

void MapScene::WriteExport(std::ostream& file) const
{
    tileset->WriteExport(file);
    for (const auto& map : maps)
        map.WriteExport(file);
}

Sprite::Sprite(const Image16Bpp& image, std::shared_ptr<Palette> global_palette)
{
    Set(image, global_palette);
}

Sprite::Sprite(const Image16Bpp& image, int bpp)
{
    name = image.name;
    width = image.width / 8;
    height = image.height / 8;
    data.resize(width * height);
    offset = 0;

    unsigned int key = (log2(width) << 2) | log2(height);
    shape = sprite_shapes[key];
    size = sprite_sizes[key];
    if (size == -1)
    {
        std::stringstream oss;
        oss << "[FATAL] Invalid sprite size (" << width << ", " << height << ")\nPlease fix.\n";
        throw oss.str();
    }

    // bpp reduce minus one for the transparent color
    int num_colors = (1 << bpp) - 1;
    const unsigned short* imgdata = image.pixels.data();
    unsigned int size = image.pixels.size();
    int weights[4] = {25, 25, 25, 25};

    std::vector<Color> pixels;
    pixels.reserve(size);

    std::vector<Color> paletteArray;
    paletteArray.reserve(num_colors + 1);

    for (unsigned int i = 0; i < size; i++)
    {
        unsigned short pix = imgdata[i];
        if (pix != params.transparent_color)
            pixels.push_back(Color(pix));
    }
    paletteArray.push_back(Color(params.transparent_color));
    MedianCut(pixels, num_colors, paletteArray, weights);

    palette.reset(new Palette(paletteArray, ""));

    std::vector<unsigned char> data4bpp(size);
    for (unsigned int i = 0; i < size; i++)
    {
        unsigned short pix = imgdata[i];
        data4bpp[i] = (pix != params.transparent_color) ? palette->Search(pix) : 0;
    }

    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;

        data[i].Set(data4bpp, image.width, tilex, tiley, 0, 4);
        data[i].palette = *palette;
    }
}

void Sprite::Set(const Image16Bpp& image, std::shared_ptr<Palette> global_palette)
{
    name = image.name;
    width = image.width / 8;
    height = image.height / 8;
    data.resize(width * height);
    offset = 0;
    palette = global_palette;

    unsigned int key = (log2(width) << 2) | log2(height);
    shape = sprite_shapes[key];
    size = sprite_sizes[key];
    if (size == -1)
    {
        std::stringstream oss;
        oss << "[FATAL] Invalid sprite size (" << width << ", " << height << ")\nPlease fix.\n";
        throw oss.str();
    }

    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;

        data[i].Set(image, *global_palette, tilex, tiley);
    }
}

void Sprite::UsePalette(const PaletteBank& bank)
{
    for (auto& tile : data)
        tile.UsePalette(bank);
    palette->Set(bank.colors);
}

void Sprite::WriteTile(unsigned char* arr, int x, int y) const
{
    int index = y * width + x;
    const GBATile& tile = data[index];
    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        arr[i] = tile.data[i];
    }
}

void Sprite::WriteExport(std::ostream& file) const
{
    if (params.bpp == 4)
        WriteDefine(file, name, "_PALETTE", palette_bank, 12);
    WriteDefine(file, name, "_SHAPE", shape, 14);
    WriteDefine(file, name, "_SIZE", size, 14);
    WriteDefine(file, name, "_ID", offset | (params.for_bitmap ? 512 : 0));
    WriteNewLine(file);
}

std::ostream& operator<<(std::ostream& file, const Sprite& sprite)
{
    for (unsigned int i = 0; i < sprite.data.size(); i++)
    {
        file << sprite.data[i];
        if (i != sprite.data.size() - 1)
            file << ",\n\t";
    }
    return file;
}

bool BlockSize::operator==(const BlockSize& rhs) const
{
    return width == rhs.width && height == rhs.height;
}

bool BlockSize::operator<(const BlockSize& rhs) const
{
    if (width != rhs.width)
        return width < rhs.width;
    else
        return height < rhs.height;
}

std::vector<BlockSize> BlockSize::BiggerSizes(const BlockSize& b)
{
    switch(b.Size())
    {
    case 1:
        return {BlockSize(2, 1), BlockSize(1, 2)};
    case 2:
        if (b.width == 2)
            return {BlockSize(4, 1), BlockSize(2, 2)};
        else
            return {BlockSize(1, 4), BlockSize(2, 2)};
    case 4:
        if (b.width == 4)
            return {BlockSize(4, 2)};
        else if (b.height == 4)
            return {BlockSize(2, 4)};
        else
            return {BlockSize(4, 2), BlockSize(2, 4)};
    case 8:
        return {BlockSize(4, 4)};
    case 16:
        return {BlockSize(8, 4), BlockSize(4, 8)};
    case 32:
        return {BlockSize(8, 8)};
    default:
        return {};
    }
}

Block Block::VSplit()
{
    size.height /= 2;
    return Block(x, y + size.height, size);
}

Block Block::HSplit()
{
    size.width /= 2;
    return Block(x + size.width, y, size);
}

Block Block::Split(const BlockSize& to_this_size)
{
    if (size.width == to_this_size.width)
        return VSplit();
    else if (size.height == to_this_size.height)
        return HSplit();
    else
        throw "Error Block::Split";

    return Block();
}

bool SpriteCompare(const Sprite& lhs, const Sprite& rhs)
{
    if (lhs.Size() != rhs.Size())
        return lhs.Size() > rhs.Size();
    else
        // Special case 2x2 should be of lesser priority than 4x1/1x4
        // This is because 2x2's don't care how a 4x4 block is split they are happy with a 4x2 or 2x4
        // Whereas 4x1 must get 4x2 and 1x4 must get 2x4.
        return lhs.width + lhs.height > rhs.width + rhs.height;
}

SpriteSheet::SpriteSheet(const std::vector<Sprite>& _sprites, const std::string& _name, int _bpp, bool _spriteSheetGiven) :
    sprites(_sprites), name(_name), bpp(_bpp), spriteSheetGiven(_spriteSheetGiven)
{
    width = bpp == 4 ? 32 : 16;
    height = !params.for_bitmap ? 32 : 16;
    data.resize(width * 8 * height * 8);
}

void SpriteSheet::Compile()
{
    PlaceSprites();
    for (const auto& block : placedBlocks)
    {
        for (unsigned int i = 0; i < block.size.height; i++)
        {
            for (unsigned int j = 0; j < block.size.width; j++)
            {
                int x = block.x + j;
                int y = block.y + i;
                int index = y * width + x;
                Sprite& sprite = sprites[block.sprite_id];
                sprite.WriteTile(data.data() + index * 64, j, i);
            }
        }
    }
}

void SpriteSheet::WriteData(std::ostream& file) const
{
    if (bpp == 8)
        WriteShortArray(file, name, "", data, 8);
    else
        WriteShortArray4Bit(file, name, "", data, 8);
    WriteNewLine(file);
}

void SpriteSheet::WriteExport(std::ostream& file) const
{
    unsigned int size = data.size() / (bpp == 4 ? 4 : 2);
    WriteExternShortArray(file, name, "", size);
    WriteDefine(file, name, "_SIZE", size);
    WriteNewLine(file);

    if (!spriteSheetGiven)
    {
        for (const auto& sprite : sprites)
        {
            sprite.WriteExport(file);
        }
    }
}

void SpriteSheet::PlaceSprites()
{
    if (spriteSheetGiven)
    {
        for (unsigned int i = 0; i < sprites.size(); i++)
        {
            Block allocd(i % width, i / width, BlockSize(1, 1));
            allocd.sprite_id = i;
            sprites[i].offset = (allocd.y * width + allocd.x) * (params.bpp == 4 ? 1 : 2);
            placedBlocks.push_back(allocd);
        }

        return;
    }
    // Gimme some blocks.
    BlockSize size(8, 8);
    for (unsigned int y = 0; y < height; y += 8)
    {
        for (unsigned int x = 0; x < width; x += 8)
        {
            freeBlocks[size].push_back(Block(x, y, size));
        }
    }

    // Sort by request size
    std::sort(sprites.begin(), sprites.end(), SpriteCompare);

    for (unsigned int i = 0; i < sprites.size(); i++)
    {
        Sprite& sprite = sprites[i];
        BlockSize size(sprite.width, sprite.height);
        std::list<BlockSize> slice;

        // Mother may I have block of this size?
        if (AssignBlockIfAvailable(size, sprite, i))
            continue;

        if (size.isBiggestSize())
        {
            std::stringstream oss;
            oss << "[FATAL] Out of sprite memory could not allocate sprite " << sprite.name << " size (" << sprite.width << "," << sprite.height << ")";
            throw oss.str();
        }

        slice.push_front(size);
        while (!HasAvailableBlock(size))
        {
            std::vector<BlockSize> sizes = BlockSize::BiggerSizes(size);
            if (sizes.empty())
            {
                std::stringstream oss;
                oss << "[FATAL] Out of sprite memory could not allocate sprite " << sprite.name << " size (" << sprite.width << "," << sprite.height << ")";
                throw oss.str();
            }

            // Default next search size will be last.
            size = sizes.back();
            for (auto& new_size : sizes)
            {
                if (HasAvailableBlock(new_size))
                {
                    size = new_size;
                    break;
                }
            }
            if (!HasAvailableBlock(size))
                slice.push_front(size);
        }

        if (!HasAvailableBlock(size))
        {
            std::stringstream oss;
            oss << "[FATAL] Out of sprite memory could not allocate sprite " << sprite.name << " size (" << sprite.width << "," << sprite.height << ")";
            throw oss.str();
        }

        SliceBlock(size, slice);

        size = BlockSize(sprite.width, sprite.height);
        // Mother may I have block of this size?
        if (AssignBlockIfAvailable(size, sprite, i))
            continue;
        else
        {
            std::stringstream oss;
            oss << "[FATAL] Out of sprite memory could not allocate sprite " << sprite.name << " size (" << sprite.width << "," << sprite.height << ")";
            throw oss.str();
        }
    }
}

bool SpriteSheet::AssignBlockIfAvailable(BlockSize& size, Sprite& sprite, unsigned int i)
{
    if (HasAvailableBlock(size))
    {
        // Yes you may deary.
        Block allocd = freeBlocks[size].front();
        freeBlocks[size].pop_front();
        allocd.sprite_id = i;
        sprite.offset = (allocd.y * width + allocd.x) * (params.bpp == 4 ? 1 : 2);
        placedBlocks.push_back(allocd);
        return true;
    }

    return false;
}

bool SpriteSheet::HasAvailableBlock(const BlockSize& size)
{
    return !freeBlocks[size].empty();
}

void SpriteSheet::SliceBlock(const BlockSize& size, const std::list<BlockSize>& slice)
{
    Block toSplit = freeBlocks[size].front();
    freeBlocks[size].pop_front();

    for (const auto& split_size : slice)
    {
        Block other = toSplit.Split(split_size);
        freeBlocks[split_size].push_back(other);
    }

    freeBlocks[toSplit.size].push_front(toSplit);
}

SpriteScene::SpriteScene(const std::vector<Image16Bpp>& images, const std::string& _name, bool _is2d, int _bpp) : name(_name), bpp(_bpp), is2d(_is2d), spriteSheetGiven(false)
{
    switch(bpp)
    {
    case 4:
        Init4bpp(images);
        break;
    default:
        Init8bpp(images);
        break;
    }
}

SpriteScene::SpriteScene(const Image16Bpp& spriteSheet, const std::string& _name, bool _is2d, int _bpp) :
    name(_name), bpp(_bpp), is2d(_is2d), spriteSheetGiven(true)
{
    InitSpriteSheet(spriteSheet);
}

SpriteScene::SpriteScene(const std::vector<Image16Bpp>& images, const std::string& _name, bool _is2d, std::shared_ptr<Palette> _palette) :
    name(_name), bpp(8), palette(_palette), is2d(_is2d), spriteSheetGiven(false)
{
    Init8bpp(images);
}

SpriteScene::SpriteScene(const std::vector<Image16Bpp>& images, const std::string& _name, bool _is2d, const std::vector<PaletteBank>& _paletteBanks) :
    name(_name), bpp(4), paletteBanks(_paletteBanks), is2d(_is2d), spriteSheetGiven(false)
{
    Init4bpp(images);
}

void SpriteScene::Build()
{
    if (is2d)
    {
        spriteSheet.reset(new SpriteSheet(sprites, name, bpp, spriteSheetGiven));
        spriteSheet->Compile();
    }
    else
    {
        std::sort(sprites.begin(), sprites.end(), SpriteCompare);
        unsigned int offset = 0;
        for (auto& sprite : sprites)
        {
            sprite.offset = offset;
            offset += sprite.width * sprite.height * (bpp == 8 ? 2 : 1);
        }
    }
}

unsigned int SpriteScene::Size() const
{
    unsigned int total = 0;
    for (const auto& sprite : sprites)
        total += sprite.width * sprite.height;

    return total * (bpp == 4 ? TILE_SIZE_SHORTS_4BPP : TILE_SIZE_SHORTS_8BPP);
}

void SpriteScene::WriteData(std::ostream& file) const
{
    if (bpp == 4)
        WriteDataPaletteBanks(file, name, paletteBanks);
    else
        palette->WriteData(file);

    if (is2d)
    {
        spriteSheet->WriteData(file);
    }
    else
    {
        file << "const unsigned short " << name << "[" << Size() << "] =\n{\n\t";
        for (unsigned int i = 0; i < sprites.size(); i++)
        {
            file << sprites[i];
            if (i != sprites.size() - 1)
                file << ",\n\t";
        }
        file << "\n};\n";
        WriteNewLine(file);
    }
}

void SpriteScene::WriteExport(std::ostream& file) const
{
    WriteDefine(file, name, "_PALETTE_TYPE", bpp == 8, 13);
    WriteDefine(file, name, "_DIMENSION_TYPE", !is2d, 6);
    WriteNewLine(file);

    if (bpp == 4)
        WriteExportPaletteBanks(file, name, paletteBanks);
    else
        palette->WriteExport(file);


    if (is2d)
    {
        spriteSheet->WriteExport(file);
    }
    else
    {
        WriteExternShortArray(file, name, "", Size());
        WriteDefine(file, name, "_SIZE", Size());
        WriteNewLine(file);

        if (!spriteSheetGiven)
        {
            for (const auto& sprite : sprites)
            {
                sprite.WriteExport(file);
            }
        }
    }
}

bool SpritePaletteSizeComp(const Sprite& i, const Sprite& j)
{
    return i.palette->Size() > j.palette->Size();
}

void SpriteScene::Init4bpp(const std::vector<Image16Bpp>& images)
{
    // TODO handle this case.  Force each sprite to choose their palette instead of making their own
    if (!paletteBanks.empty())
    {
        //For each image reduce to 4bpp and set in palette banks.
    }
    else
    {
        // Initialize Palette Banks
        paletteBanks.reserve(16);
        for (int i = 0; i < 16; i++)
            paletteBanks.push_back(PaletteBank(i));

        // Form sprites
        for (const auto& image : images)
            sprites.push_back(Sprite(image, bpp));

        // Palette bank selection time
        // Ensure image contains < 256 colors
        std::set<Color> bigPalette;
        for (const auto& sprite : sprites)
        {
            const std::vector<Color>& sprite_palette = sprite.palette->colors;
            bigPalette.insert(sprite_palette.begin(), sprite_palette.end());
        }

        if (bigPalette.size() > 256 && !params.force)
        {
            std::stringstream oss;
            oss << "[ERROR] Image after reducing sprites to 4bpp still contains more than 256 distinct colors.  Found " << bigPalette.size() << " colors. Please fix.";
            throw oss.str();
        }

        // Greedy approach deal with tiles with largest palettes first.
        std::sort(sprites.begin(), sprites.end(), SpritePaletteSizeComp);

        // But deal with transparent color
        for (unsigned int i = 0; i < paletteBanks.size(); i++)
            paletteBanks[i].Add(Color(params.transparent_color));

        // Construct palette banks, assign bank id to tile, remap sprite to palette bank given, assign tile ids
        for (auto& sprite : sprites)
        {
            int pbank = -1;
            // Fully contains checks
            for (unsigned int i = 0; i < paletteBanks.size(); i++)
            {
                PaletteBank& bank = paletteBanks[i];
                if (bank.Contains(*sprite.palette))
                    pbank = i;
            }

            // Ok then find least affected bank
            if (pbank == -1)
            {
                int max_colors_left = -1;
                for (unsigned int i = 0; i < paletteBanks.size(); i++)
                {
                    PaletteBank& bank = paletteBanks[i];
                    int colors_left = bank.CanMerge(*sprite.palette);
                    if (colors_left != 0 && max_colors_left < colors_left)
                    {
                        max_colors_left = colors_left;
                        pbank = i;
                    }
                }
            }
            // Cry and die for now. Unless you tell me to keep going.
            if (pbank == -1 && !params.force)
                throw "More than 16 distinct palettes found, please use 8bpp mode.";

            // Merge step and assign palette bank
            paletteBanks[pbank].Merge(*sprite.palette);
            sprite.palette_bank = pbank;
            sprite.UsePalette(paletteBanks[pbank]);
        }
    }
}

void SpriteScene::Init8bpp(const std::vector<Image16Bpp>& images)
{
    if (!palette)
    {
        Image8BppScene scene(images, name);
        palette = scene.palette;
    }

    for (const auto& image : images)
    {
        sprites.push_back(Sprite(image, palette));
    }
}

void SpriteScene::InitSpriteSheet(const Image16Bpp& sheet)
{
    std::vector<Image16Bpp> tiles;
    int tilesX = sheet.width / 8;
    int tilesY = sheet.height / 8;
    tiles.reserve(tilesX * tilesY);
    for (int y = 0; y < tilesY; y++)
    {
        for (int x = 0; x < tilesX; x++)
        {
            tiles.push_back(sheet.SubImage(x * 8, y * 8, 8, 8));
        }
    }

    if (bpp == 4)
        Init4bpp(tiles);
    else
        Init8bpp(tiles);
}
