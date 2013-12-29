#include "headerfile.hpp"

#include <cstdio>
#include <map>
#include <vector>

#include "fileutils.hpp"
#include "shared.hpp"

HeaderFile header;

const char* types[4] = {"_BITMAP_H", "_TILEMAP_H", "_TILESET_H", "_MAP_H"};

void HeaderFile::Write(std::ostream& file)
{
    ExportFile::Write(file);

    std::vector<std::string> names;
    std::vector<std::string> animated_extras;

    WriteHeaderGuard(file, params.name, types[type]);
    bool ok_newline = false;
    if (params.transparent_given)
    {
        char buffer[7];
        sprintf(buffer, "0x%04x", transparent_color);
        WriteDefine(file, params.name, "_TRANSPARENT", (mode == 3) ? buffer : "0x00");
        ok_newline = true;
    }
    if (params.offset)
    {
        WriteDefine(file, params.name, "_PALETTE_OFFSET ", params.offset);
        ok_newline = true;
    }
    if (ok_newline) WriteNewLine(file);

    for (const auto& image_ptr : images16)
    {
        const Image16Bpp& image = *image_ptr;
        image.WriteExport(file);

        names.push_back(image.name);
    }

    for (const auto& image_ptr : images8)
    {
        const Image8Bpp& image = *image_ptr;
        const Palette& palette = *image.palette;

        palette.WriteExport(file);
        image.WriteExport(file);

        names.push_back(image.name);
    }
    if (!images8.empty())
        animated_extras.push_back("_palette");

    if (image8Scene)
    {
        const Image8BppScene& scene = *image8Scene;
        scene.WriteExport(file);

        for (const auto& image : scene.images)
            names.push_back(image.name);
    }

    for (const auto& map_ptr : mapSets)
    {
        const Map& map = *map_ptr;
        const Tileset& tileset = *map.tileset;

        tileset.WriteExport(file);
        map.WriteExport(file);

        names.push_back(map.name + "_map");
    }
    if (!mapSets.empty())
    {
        animated_extras.push_back("_tiles");
        animated_extras.push_back("_palette");
    }

    if (mapScene)
    {
        const MapScene& scene = *mapScene;
        scene.WriteExport(file);

        for (const auto& map : scene.maps)
            names.push_back(map.name + "_map");
    }

    if (tileset)
    {
        const Tileset& tiles = *tileset;
        tiles.WriteExport(file);
    }

    for (const auto& map_ptr : maps)
    {
        const Map& map = *map_ptr;
        map.WriteExport(file);
        names.push_back(map.name + "_map");
    }

    if (params.animated && names.size() > 1)
    {
        WriteExternShortPtrArray(file, params.name, "_frames", names.size());
        WriteDefine(file, params.name, "_ANIMATION_FRAMES", names.size());
        WriteNewLine(file);
        for (const std::string& extra : animated_extras)
        {
            std::string extra_name = params.name + extra;
            WriteExternShortPtrArray(file, extra_name, "_frames", names.size());
            WriteDefine(file, extra_name, "_ANIMATION_FRAMES", names.size());
        }
    }

    WriteEndHeaderGuard(file);
    WriteNewLine(file);
}
