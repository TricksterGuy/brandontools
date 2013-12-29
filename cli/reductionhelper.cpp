#include "reductionhelper.hpp"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <unordered_set>

#include "cpercep.hpp"
#include "dither.hpp"
#include "fileutils.hpp"
#include "mediancut.hpp"
#include "shared.hpp"

uint32_t hashlittle( const void *key, size_t length, uint32_t initval);

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
    colors.resize(width * height);
    std::vector<Color>::iterator it = colors.begin();
    GetColors(it);
}

void Image16Bpp::GetColors(std::vector<Color>::iterator& color_ptr) const
{
    for (unsigned int i = 0; i < width * height; i++)
    {
        short pix = pixels[i];
        color_ptr->Set(pix & 0x1f, (pix >> 5) & 0x1f, (pix >> 10) & 0x1f);
        color_ptr++;
    }
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
    if (params.transparent_color != -1)
    {
        int trans_index = Search(params.transparent_color);
        Color temp = colors[trans_index];
        colors[trans_index] = colors[0];
        colors[0] = temp;
    }

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
    Color color;
    int r, g, b;
    r = color_data & 0x1f;
    g = (color_data >> 5) & 0x1f;
    b = (color_data >> 10) & 0x1f;

    color.Set(r, g, b);
    return Search(color);
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

    std::vector<Color> paletteColors;
    MedianCut(pixels16, params.palette, paletteColors, params.weights);
    palette.reset(new Palette(paletteColors, name));
    RiemersmaDither(pixels16.begin(), *this, params.dither, params.dither_level);
    if (params.offset > 0)
    {
        for (unsigned char& pix : pixels)
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

    std::vector<Color> pixels16;
    image.GetColors(pixels16);

    RiemersmaDither(pixels16.begin(), *this, params.dither, params.dither_level);
    if (params.offset > 0)
    {
        for (unsigned char& pix : pixels)
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

    std::vector<Color> pixels(total_pixels);
    std::vector<Color>::iterator iterator = pixels.begin();
    for (unsigned int i = 0; i < images16.size(); i++)
        images16[i].GetColors(iterator);

    std::vector<Color> paletteColors;
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
        printf("[WARNING] Color remap: Color given to palette bank not an exact match.\n");
    }

    return index;
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
    // bpp reduce
    int num_colors = 1 << bpp;
    const unsigned short* imgdata = imageTile->data.data();
    int weights[4] = {25, 25, 25, 25};

    std::vector<Color> paletteArray;
    std::vector<Color> pixels(TILE_SIZE);

    for (unsigned int i = 0; i < TILE_SIZE; i++)
    {
        unsigned short pix = imgdata[i];
        pixels[i].Set(pix & 0x1f, (pix >> 5) & 0x1f, (pix >> 10) & 0x1f);
    }

    MedianCut(pixels, num_colors, paletteArray, weights);

    palette.Set(paletteArray);

    for (unsigned int i = 0; i < TILE_SIZE; i++)
        data[i] = palette.Search(pixels[i]);

    sourceTile = imageTile;
}

template <>
void Tile<unsigned char>::UsePalette(const PaletteBank& bank)
{
    if (bpp == 8)
    {
        printf("Not implemented");
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

struct GBATileHash
{
    std::size_t operator()(const GBATile& k) const
    {
        return hashlittle(k.data.data(), TILE_SIZE, 0xDEADDEAD);
    }
};

struct GBATile4bppEqual
{
    bool operator()(const GBATile& lhs, const GBATile& rhs) const
    {
        return lhs.data == rhs.data && lhs.palette_bank == rhs.palette_bank;
    }
};

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
        return foundTile->id;

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
    palette->WriteData(file);
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
    palette->WriteExport(file);
    WriteDefine(file, name, "_PALETTE_TYPE", (bpp == 4) ? "(0 << 7)" : "(1 << 7)");
    WriteNewLine(file);

    WriteExternShortArray(file, name, "_tiles", Size());
    WriteDefine(file, name, "_TILES", tiles.size());
    WriteDefine(file, name, "_TILES_SIZE", Size());
    WriteNewLine(file);
}

void Tileset::Init4bpp(const std::vector<Image16Bpp>& images)
{
    // Tile image into 16 bit tiles
    Tileset tileset16bpp(images, name, 16);
    std::set<ImageTile> imageTiles = tileset16bpp.itiles;

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

    if (bigPalette.size() > 256)
    {
        throw "[ERROR] Image after reducing tiles to 4bpp still contains more than 256 distinct colors. Please fix.";
    }

    // Greedy approach deal with tiles with largest palettes first.
    std::sort(gbaTiles.begin(), gbaTiles.end(), TilesPaletteSizeComp);

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

        // Cry and die for now.
        if (pbank == -1)
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
    printf("[INFO] Tiles found %zu.\n", tiles.size());
    printf("[INFO] Tiles uses %d charblocks and %d screenblocks.\n", cbbs, sbbs);
    printf("[INFO] Total utilization %.2f/4 charblocks or %d/32 screenblocks, %d/65536 bytes.\n",
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
    tiles.insert(nullTile);
    tilesExport.push_back(nullTile);

    for (unsigned int k = 0; k < images.size(); k++)
    {
        bool disjoint_error = false;
        const Image8Bpp& image = images[k];

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
            }
            else if (offsets.size() > 1 && !disjoint_error)
            {
                printf("[WARNING] Tiles found in tileset image %s are not disjoint, offset calculations may be off\n", image.name.c_str());
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
    printf("[INFO] Tiles found %zu.\n", tiles.size());
    printf("[INFO] Tiles uses %d charblocks and %d screenblocks.\n", cbbs, sbbs);
    printf("[INFO] Total utilization %.2f/4 charblocks or %d/32 screenblocks, %d/65536 bytes.\n",
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
        default:
            Init8bpp(image);
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
            printf("[WARNING] Image: %s No match for tile starting at (%d %d) px, using empty tile instead.\n", image.name.c_str(), tilex * 8, tiley * 8);
            printf("[WARNING] Image: %s No match for palette for tile starting at (%d %d) px, using palette 0 instead.\n", image.name.c_str(), tilex * 8, tiley * 8);
        }

        data[i] = pal_id << 12 | tile_id;
    }
}

void Map::Init8bpp(const Image16Bpp& image16)
{
    Image8Bpp image(image16);
    const std::vector<unsigned char>& pixels = image.pixels;

    for (unsigned int i = 0; i < data.size(); i++)
    {
        int tilex = i % width;
        int tiley = i / width;
        GBATile tile(pixels, image.width, tilex, tiley);
        int tile_id = tileset->Search(tile);

        if (tile_id == -1)
        {
            printf("[WARNING] Image: %s No match for tile starting at (%d %d) px, using empty tile instead.\n", image.name.c_str(), tilex * 8, tiley * 8);
            tile_id = 0;
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
    WriteDefine(file, name, "_MAP_TYPE", Type());
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
