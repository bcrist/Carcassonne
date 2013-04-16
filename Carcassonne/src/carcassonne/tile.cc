// Copyright (c) 2013 Dougrist Productions
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Author: Benjamin Crist
// File: carcassonne/tile.cc
//
// Represents a single game tile which can be placed in a draw pile or on the
// board.

#include "carcassonne/tile.h"

namespace carcassonne {

TileEdge::TileEdge()
   : type(TYPE_FARM),
     open(true),
     farm(nullptr),
     cw_farm(nullptr),
	  ccw_farm(nullptr)
{
}

TileEdge::TileEdge(features::Farm* farm)
	: type(TYPE_FARM),
	  open(true),
	  farm(farm),
	  cw_farm(nullptr),
	  ccw_farm(nullptr)
{
}

TileEdge::TileEdge(features::Road* road, features::Farm* cw_farm, features::Farm* ccw_farm)
	: type(TYPE_ROAD),
	  open(true),
	  road(road),
	  cw_farm(cw_farm),
	  ccw_farm(ccw_farm)
{
}

TileEdge::TileEdge(features::City* city)
	: type(TYPE_CITY),
	  open(true),
	  city(city),
	  cw_farm(nullptr),
	  ccw_farm(nullptr)
{
}


// Constructs a tile of one of the TYPE_EMPTY_* types
Tile::Tile(Type type)
	: color_(1,1,1,1),
	  mesh_(nullptr)
{
}

// Sets the tile's type.  A TYPE_EMPTY_* tile can only be set to any of the
// other TYPE_EMPTY_* types.  A TYPE_PLACED tile can't be changed to any
// other type.  A TYPE_FLOATING tile can only be changed to TYPE_PLACED.
void Tile::setType(Type type)
{
}


// Rotates the tile if it is a TYPE_FLOATING tile
void Tile::rotateClockwise()
{
}

void Tile::rotateCounterclockwise()
{
}

// Returns the type of features which currently exist on the requested side.
const TileEdge& Tile::getEdge(Side side)
{
   return edges_[0];
}

std::vector<features::Feature*> Tile::getFeatures()
{
   return std::vector<features::Feature*>();
}

// called when a tile is placed 
void Tile::closeSide(Side side, Tile* new_neighbor)
{
}

void Tile::closeDiagonal(Side clockwise_from, Tile* new_diagonal_neighbor)
{
}


void Tile::draw() const
{
}

void Tile::replaceCity(const features::City& old_city, const features::City& new_city)
{
}

void Tile::replaceFarm(const features::Farm& old_farm, const features::Farm& new_farm)
{
}

void Tile::replaceRoad(const features::Road& old_road, const features::Road& new_road)
{
}
   
} // namespace carcassonne
