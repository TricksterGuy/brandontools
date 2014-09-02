#include "implementationfile.hpp"

#include <map>
#include <vector>

#include "fileutils.hpp"
#include "shared.hpp"

ImplementationFile implementation;

void ImplementationFile::Write(std::ostream& file)
{
    ExportFile::Write(file);
    std::vector<std::string> names;
    std::vector<std::string> animated_extras;

    for (const auto& image_ptr : images16)
    {
        const Image16Bpp& image = *image_ptr;
        image.WriteData(file);

        names.push_back(image.name);
    }

    for (const auto& image_ptr : images8)
    {
        const Image8Bpp& image = *image_ptr;
        const Palette& palette = *image.palette;

        palette.WriteData(file);
        image.WriteData(file);

        names.push_back(image.name);
    }
    if (!images8.empty())
        animated_extras.push_back("_palette");

    if (image8Scene)
    {
        const Image8BppScene& scene = *image8Scene;
        scene.WriteData(file);

        for (const auto& image : scene.images)
            names.push_back(image.name);
    }

    for (const auto& map_ptr : mapSets)
    {
        const Map& map = *map_ptr;
        const Tileset& tileset = *map.tileset;

        tileset.WriteData(file);
        map.WriteData(file);

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
        scene.WriteData(file);

        for (const auto& map : scene.maps)
            names.push_back(map.name + "_map");
    }

    if (tileset)
    {
        const Tileset& tiles = *tileset;
        tiles.WriteData(file);
    }

    for (const auto map_ptr : maps)
    {
        const Map& map = *map_ptr;
        map.WriteData(file);
        names.push_back(map.name + "_map");
    }

    if (spriteScene)
    {
        const SpriteScene& scene = *spriteScene;
        scene.WriteData(file);
    }

    if (params.animated)
    {
        WriteShortPtrArray(file, params.filename, "_frames", names, 1);
        WriteNewLine(file);
        for (const std::string& extra : animated_extras)
        {
            std::string extra_name = params.filename + extra;
            WriteShortPtrArray(file, extra_name, "_frames", names, extra, 1);
        }
    }
}
