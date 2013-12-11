#ifndef REDUCTION_HELPER_HPP
#define REDUCTION_HELPER_HPP

#include <vector>
#include <iostream>
#include <Magick++.h>
#include "shared.hpp"
#include "cpercep.hpp"
#include "mediancut.hpp"
#include "tile.hpp"

#define TOTAL_TILE_MEMORY_BYTES 65536
#define SIZE_CBB_BYTES (8192 * 2)
#define SIZE_SBB_BYTES (1024 * 2)
#define SIZE_SBB_SHORTS 1024

typedef std::vector<unsigned char> IndexedImage;

extern std::vector<Color> palette;

Magick::Image ConvertToGBA(Magick::Image image);

int paletteSearch(Color a);
void QuantizeImage(Magick::Image image, const ExportParams& params, IndexedImage& indexedImage);
void RiemersmaDither(std::vector<Color>::iterator image, IndexedImage& indexedImage, int width, int height, int dither, float ditherlevel);
void FormPaletteAndIndexedImages(std::vector<Magick::Image> images, const ExportParams& params, std::vector<IndexedImage>& indexedImages);

void GetTiles4bpp(std::vector<Magick::Image> images, const ExportParams& params, std::vector<Tile>& tiles, std::vector<short>& offsets);
void GetTiles8bpp(std::vector<Magick::Image> images, const ExportParams& params, std::vector<Tile>& tiles, std::vector<short>& offsets);

unsigned short PackPixels(const void* pixelArray, unsigned int i, const void* unused);
unsigned short GetPaletteEntry(const void* paletteArray, unsigned int i, const void* num_colors);
unsigned short GetIndexedEntry(const void* indicesArray, unsigned int i, const void* offset_ptr);
std::string getAnimFrameName(const void* stringArray, unsigned int i, const void* unused);

void WriteTiles(std::ostream& file, const ExportParams& params, const std::string& name, const std::vector<Tile>& tiles);

#ifndef CLAMP
#define CLAMP(x) (((x) < 0.0) ? 0.0 : (((x) > 31) ? 31 : (x)))
#endif

#endif
