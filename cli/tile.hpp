#ifndef TILE_HPP
#define TILE_HPP

#include <iostream>
#include "shared.hpp"

#define TILE_SIZE 64
#define PALETTE_SIZE 16
#define TILE_SIZE_BYTES_8BPP 64
#define TILE_SIZE_BYTES_4BPP 32

class Tile
{
	public:
		Tile(unsigned char* data_ptr = NULL, unsigned short* palette_ptr = NULL, unsigned short tile_id = 0);
		Tile(const std::vector<unsigned char>& indexedImage, int pitch, int tilex, int tiley);
		Tile(const std::vector<Color>& Image, int pitch, int tilex, int tiley);
		~Tile();
        void Set(unsigned char* data_ptr = NULL, unsigned short* palette_ptr = NULL);
        void Set(const std::vector<unsigned char>& indexedImage, int pitch, int tilex, int tiley);
        void Set(const std::vector<Color>& Image, int pitch, int tilex, int tiley);
        // Is the tile exactly the same as other
        bool IsEqual(const Tile& other) const;
        // Is the tile the same data as other except flipped.
		bool IsSameAs(const Tile& other) const;
		bool operator<(const Tile& other) const;
		bool operator==(const Tile& other) const;

		unsigned short id;
	private:
        std::vector<unsigned char> data;
        // unused if bpp == 8.
        std::vector<unsigned short> palette;

    friend std::ostream& operator<<(std::ostream& file, const Tile& tile);
};

extern Tile NULLTILE;

#endif
