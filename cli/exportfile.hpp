#ifndef EXPORT_FILE_HPP
#define EXPORT_FILE_HPP

#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <vector>

#include "reductionhelper.hpp"

enum
{
    TYPE_BITMAP = 0,
    TYPE_TILEMAP = 1,
    TYPE_TILESET = 2,
    TYPE_MAP = 3,
};

class ExportFile
{
    public:
        ExportFile(const std::string& _invocation = "") : invocation(_invocation), transparent_color(-1), mode(3),
            type(0), palette(NULL), image8Scene(NULL), mapScene(NULL), tileset(NULL), spriteScene(NULL) {};
        virtual ~ExportFile() {};

        void SetInvocation(const std::string& invo) {invocation = invo;};
        void SetTransparent(int p_color) {transparent_color = p_color;};
        void SetMode(int _mode) {mode = _mode;};
        void SetTilesets(const std::vector<std::string>& _tilesets) {tilesets = _tilesets;};
        void SetPalette(std::shared_ptr<Palette> _palette) {palette = _palette;};
        void SetType(int _type) {type = _type;};

        virtual void Write(std::ostream& file);
        void AddLine(const std::string& line);
        void AddImageInfo(const std::string& filename, int scene, int width, int height, bool frame);

        void AddImage(std::shared_ptr<Image32Bpp> image);
        void AddImage(std::shared_ptr<Image16Bpp> image);
        void AddImage(std::shared_ptr<Image8Bpp> image);
        void AddScene(std::shared_ptr<Image8BppScene> images);
        void AddMapSet(std::shared_ptr<Map> mapSet);
        void AddScene(std::shared_ptr<MapScene> mapScene);
        void AddTileset(std::shared_ptr<Tileset> tileset);
        void AddMap(std::shared_ptr<Map> map);
        void AddScene(std::shared_ptr<SpriteScene> sprites);

    private:
        std::string invocation;
        std::vector<std::string> lines;
        std::vector<std::string> imageInfos;
        std::vector<std::string> tilesets;
        // Cases
        // GBA
        // 1) 1 16 bit image
        // 2) multi 16 bit images
        // 3) 1 8 bit image
        // 4) multi 8 bit images each with own palette
        // 5) multi 8 bit images w/ shared palette
        // 6) palette tileset map
        // 7) palette tileset map multiple sets
        // 8) palette tileset maps (palette and tileset shared)
        // 9) palette and tileset
        // 10) a map
        // 11) multiple maps
        // 12) A spritesheet (2d and 1d)
        // DS
        // 3DS
        // 1) 24 bit images

        // 1-2) images16
        // 3-4) images8
        // 5) images8scene
        // 6-7) mapSets
        // 8) mapScene
        // 9) tileset
        // 10-11) maps
        // 12) spriteScene

    protected:
        int transparent_color;
        int mode;
        int type;
        std::vector<std::shared_ptr<Image16Bpp>> images16;
        std::shared_ptr<Palette> palette;
        std::vector<std::shared_ptr<Image8Bpp>> images8;
        std::shared_ptr<Image8BppScene> image8Scene;
        std::vector<std::shared_ptr<Map>> mapSets;
        std::shared_ptr<MapScene> mapScene;
        std::shared_ptr<Tileset> tileset;
        std::vector<std::shared_ptr<Map>> maps;
        std::shared_ptr<SpriteScene> spriteScene;

        std::vector<std::shared_ptr<Image32Bpp>> images32;
};

#endif
