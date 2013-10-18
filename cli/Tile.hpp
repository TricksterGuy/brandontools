#ifndef TILE_HPP
#define TILE_HPP

#include <cstdlib>

class Tile
{
	public:
		Tile(int* data = NULL);
		~Tile();

		int IsSameAs(const Tile& other);
	private:
        int data[64];

};


#endif
