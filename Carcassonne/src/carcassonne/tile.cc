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

#include "carcassonne/asset_manager.h"

namespace carcassonne {

#pragma region TileEdge

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

#pragma endregion

// Constructs a tile of one of the TYPE_EMPTY_* types
Tile::Tile(AssetManager& asset_mgr, Type type)
	: type_(type),
     color_(1,1,1,1),
	  mesh_(asset_mgr.getMesh("std-tile")),
     texture_(nullptr),
     rotation_(ROTATION_NONE)
{
   assert(type != TYPE_PLACED && type != TYPE_FLOATING);
}

// Load tile from database
Tile::Tile(AssetManager& asset_mgr, const std::string& name)
{
   // TODO
}

// Copy another tile (does not share feature objects)
Tile::Tile(const Tile& other)
   : type_(other.type_),
     color_(other.color_),
     mesh_(other.mesh_),
     texture_(other.texture_),
     rotation_(ROTATION_NONE)
{
   for (int i = 0; i < 4; ++i)
      edges_[i] = other.edges_[i];

   for (auto i(other.cities_.begin()), end(other.cities_.end()); i != end; ++i)
   {
      const features::City& city = static_cast<const features::City&>(**i);
      features::City* new_city = new features::City(city);

      cities_.push_back(new_city->shared_from_this());

      for (int i = 0; i < 4; ++i)
      {
         TileEdge& edge = edges_[i];
         if (edge.type == TileEdge::TYPE_CITY && edge.city == &city)
            edge.city = new_city;
      }
   }

   for (auto i(other.roads_.begin()), end(other.roads_.end()); i != end; ++i)
   {
      const features::Road& road = static_cast<const features::Road&>(**i);
      features::Road* new_road = new features::Road(road);

      roads_.push_back(new_road->shared_from_this());

      for (int i = 0; i < 4; ++i)
      {
         TileEdge& edge = edges_[i];
         if (edge.type == TileEdge::TYPE_ROAD && edge.road == &road)
            edge.road = new_road;
      }
   }

   for (auto i(other.farms_.begin()), end(other.farms_.end()); i != end; ++i)
   {
      const features::Farm& farm = static_cast<const features::Farm&>(**i);
      features::Farm* new_farm = new features::Farm(farm);

      farms_.push_back(new_farm->shared_from_this());

      for (auto i(other.cities_.begin()), end(other.cities_.end()); i != end; ++i)
      {
         const features::City& city = static_cast<const features::City&>(**i);
         features::City& new_city = static_cast<features::City&>(
            **(cities_.begin() + (i - other.cities_.begin())));

         new_farm->replaceCity(city, new_city);
      }

      for (int i = 0; i < 4; ++i)
      {
         TileEdge& edge = edges_[i];
         if (edge.type == TileEdge::TYPE_ROAD)
         {
            if (edge.cw_farm == &farm)
               edge.cw_farm = new_farm;

            if (edge.ccw_farm == &farm)
               edge.ccw_farm = new_farm;
         }
         else if (edge.type == TileEdge::TYPE_FARM && edge.farm == &farm)
            edge.farm = new_farm;
      }
   }
}


// Sets the tile's type.  A TYPE_EMPTY_* tile can only be set to any of the
// other TYPE_EMPTY_* types.  A TYPE_PLACED tile can't be changed to any
// other type.  A TYPE_FLOATING tile can only be changed to TYPE_PLACED.
void Tile::setType(Type type)
{
   switch(type_)
   {
      case TYPE_EMPTY_PLACEABLE:
      case TYPE_EMPTY_PLACEABLE_IF_ROTATED:
      case TYPE_EMPTY_NOT_PLACEABLE:
         if (type == TYPE_FLOATING || type == TYPE_PLACED)
            break;
         else
            type_ = type;
         break;

      case TYPE_FLOATING:
         if (type != TYPE_PLACED)
            break;
         type_ = type;
         break;

      default:
         break;
   }
         
}

Tile::Type Tile::getType()const
{
   return type_;
}


// Rotates the tile if it is a TYPE_FLOATING tile
void Tile::rotateClockwise()
{
   if (type_ != TYPE_FLOATING)
      return;
   rotation_ = static_cast<Rotation>((static_cast<int>(rotation_) + 1) % 4);
}

void Tile::rotateCounterclockwise()
{
   if (type_ != TYPE_FLOATING)
      return;
   rotation_ = static_cast<Rotation>((static_cast<int>(rotation_) + 3) % 4);
}

void Tile::setPosition(const glm::vec3& position)
{
   position_ = position;
}

// Returns the type of features which currently exist on the requested side.
//const TileEdge& Tile::getEdge(Side side)
//{
//   return edges_[(static_cast<int>(side) + 4 - static_cast<int>(rotation_)) % 4];
//}

TileEdge& Tile::getEdge(Side side)
{
   return edges_[(static_cast<int>(side) + 4 - static_cast<int>(rotation_)) % 4];
}

/*std::vector<features::Feature*> Tile::getFeatures()
{
   return std::vector<features::Feature*>();
}*/

// called when a tile is placed
// should be called on (up to) all four sides of the new tile
void Tile::closeSide(Side side, Tile& new_neighbor)
{
   Side neighbor_side = static_cast<Side>((side + 2) % 4);
   TileEdge& neighbor_edge = new_neighbor.getEdge(neighbor_side);
   TileEdge& edge = getEdge(side);

   edge.open = false;
   neighbor_edge.open = false;

   if (neighbor_edge.type != edge.type)
   {
      std::cerr << "Warning: edge mismatch!" << std::endl;
      return;
   }

   switch (edge.type)
   {
      case TileEdge::TYPE_FARM:
         neighbor_edge.farm->join(*edge.farm);
         break;

      case TileEdge::TYPE_ROAD:
         neighbor_edge.road->join(*edge.road);
         neighbor_edge.cw_farm->join(*edge.ccw_farm);
         neighbor_edge.ccw_farm->join(*edge.cw_farm);
         break;

      case TileEdge::TYPE_CITY:
         neighbor_edge.city->join(*edge.city);
         break;

      default:
         break;
   }

   if (cloister_)
      cloister_->addTile(new_neighbor);

   if (new_neighbor.cloister_)
      new_neighbor.cloister_->addTile(*this);
}

void Tile::closeDiagonal(Tile& new_diagonal_neighbor)
{
   if (cloister_)
      cloister_->addTile(new_diagonal_neighbor);

   if (new_diagonal_neighbor.cloister_)
      new_diagonal_neighbor.cloister_->addTile(*this);
}


void Tile::draw() const
{
   // TODO!
}

void Tile::replaceCity(const features::City& old_city, features::City& new_city)
{
   for (auto i(cities_.begin()), end(cities_.end()); i!= end; ++i)
   {
      std::shared_ptr<features::Feature>& ptr = *i;

      if (&old_city == ptr.get())
         ptr = new_city.shared_from_this();
   }

   for (int i = 0; i < 4; ++i)
   {
      TileEdge& edge = edges_[i];
      if (edge.city == &old_city)
         edge.city = &new_city;
   }
}

void Tile::replaceFarm(const features::Farm& old_farm, features::Farm& new_farm)
{
   for (auto i(farms_.begin()), end(farms_.end()); i!= end; ++i)
   {
      std::shared_ptr<features::Feature>& ptr = *i;

      if (&old_farm == ptr.get())
         ptr = new_farm.shared_from_this();
   }

   for (int i = 0; i < 4; ++i)
   {
      TileEdge& edge = edges_[i];
      if (edge.farm == &old_farm)
         edge.farm = &new_farm;
   }
}

void Tile::replaceRoad(const features::Road& old_road, features::Road& new_road)
{
   for (auto i(roads_.begin()), end(roads_.end()); i!= end; ++i)
   {
      std::shared_ptr<features::Feature>& ptr = *i;

      if (&old_road == ptr.get())
         ptr = new_road.shared_from_this();
   }

   for (int i = 0; i < 4; ++i)
   {
      TileEdge& edge = edges_[i];
      if (edge.road == &old_road)
         edge.road = &new_road;
   }
}
   
} // namespace carcassonne
