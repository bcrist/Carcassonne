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
// Author: Benjamin Crist / Josh Douglas
// File: carcassonne/board.cc
//
// Represents a map of the game world - by using a std::unordered_map of tiles

#include "carcassonne/board.h"

#include "carcassonne/asset_manager.h"

namespace carcassonne {

Board::Board(AssetManager& asset_mgr)
   : asset_mgr_(asset_mgr)
{
   makeEmpty(glm::ivec2(0,0));
}

glm::ivec2 Board::getCoordinates(const glm::vec3& position) const
{
   return glm::ivec2(int(floor(position.x + 0.5f)),
                     int(floor(position.z + 0.5f)));
}

Tile* Board::getTileAt(const glm::ivec2& position) const
{
   auto i(board_.find(position));
   return i == board_.end() ? nullptr : i->second.get();
}

// If tile is a TYPE_EMPTY_* tile, return false if the position already has
// a tile of any kind.
// If tile is a TYPE_FLOATING tile, return false if the position does not
// currently have a TYPE_EMPTY_PLACEABLE tile.
bool Board::placeTileAt(const glm::ivec2& position, std::unique_ptr<Tile>&& tile)
{
   switch (tile->getType())
   {
      case Tile::TYPE_EMPTY_NOT_PLACEABLE:
      case Tile::TYPE_EMPTY_PLACEABLE_IF_ROTATED:
      case Tile::TYPE_EMPTY_PLACEABLE:
         if (getTileAt(position) != nullptr)
            return false;

         board_[position] = std::move(tile);
         return true;

      case Tile::TYPE_FLOATING:
         {
            Tile* old_tile = getTileAt(position);
            if (old_tile == nullptr || old_tile->getType() != Tile::TYPE_EMPTY_PLACEABLE)
               return false;

            empty_locations_.erase(position);
            tile->setType(Tile::TYPE_PLACED);
            tile->setPosition(glm::vec3(position.x, 0, position.y));

            Tile* north = makeEmpty(position + glm::ivec2(1, 0));
            Tile* south = makeEmpty(position + glm::ivec2(-1, 0));
            Tile* east = makeEmpty(position + glm::ivec2(0, 1));
            Tile* west = makeEmpty(position + glm::ivec2(0, -1));
            tile->closeSide(Tile::SIDE_NORTH, north);
            tile->closeSide(Tile::SIDE_SOUTH, south);
            tile->closeSide(Tile::SIDE_EAST, east);
            tile->closeSide(Tile::SIDE_WEST, west);
           
            Tile* ne = getTileAt(position + glm::ivec2(1, 1));
            Tile* nw = getTileAt(position + glm::ivec2(1, -1));
            Tile* se = getTileAt(position + glm::ivec2(-1, 1));
            Tile* sw = getTileAt(position + glm::ivec2(-1, -1));
            tile->closeDiagonal(ne);
            tile->closeDiagonal(nw);
            tile->closeDiagonal(se);
            tile->closeDiagonal(sw);

            board_[position] = std::move(tile);
            return true;
         }

      default:
         return false;
   }
}

void Board::usingNewTile(const Tile& tile)
{
   for (auto i(empty_locations_.begin()), end(empty_locations_.end()); i != end; ++i)
   {
      Tile* current = getTileAt(*i);
      checkTilePlaceable(*i, current, tile);
   }
}

void Board::tileRotated(const Tile& tile)
{
   for (auto i(empty_locations_.begin()), end(empty_locations_.end()); i != end; ++i)
   {
      Tile* current = getTileAt(*i);
      if (current->getType() == Tile::TYPE_EMPTY_NOT_PLACEABLE)
         continue;

      checkTilePlaceable(*i, current, tile);
   }
}

void Board::checkTilePlaceable(const glm::ivec2& position, Tile* current, const Tile& tile)
{
   Tile* north = getTileAt(position + glm::ivec2(1, 0));
   Tile* south = getTileAt(position - glm::ivec2(1, 0));
   Tile* east = getTileAt(position + glm::ivec2(0, 1));
   Tile* west = getTileAt(position - glm::ivec2(0, 1));

   current->setType(Tile::TYPE_EMPTY_NOT_PLACEABLE);

   // try all 4 rotations of the tile
   for (int i = 0; i < 4; ++i)
   {
      if ((north == nullptr || north->getType() != Tile::TYPE_PLACED ||
           tile.getEdge(static_cast<Tile::Side>(i + Tile::SIDE_NORTH)).type == 
           north->getEdge(Tile::SIDE_SOUTH).type) &&

          (south == nullptr || south->getType() != Tile::TYPE_PLACED ||
           tile.getEdge(static_cast<Tile::Side>(i + Tile::SIDE_SOUTH)).type ==
           south->getEdge(Tile::SIDE_NORTH).type) &&

          (east == nullptr || east->getType() != Tile::TYPE_PLACED ||
           tile.getEdge(static_cast<Tile::Side>(i + Tile::SIDE_EAST)).type == 
           east->getEdge(Tile::SIDE_WEST).type) &&
          
          (west == nullptr || west->getType() != Tile::TYPE_PLACED ||
           tile.getEdge(static_cast<Tile::Side>(i + Tile::SIDE_WEST)).type ==
           west->getEdge(Tile::SIDE_EAST).type))
      {
         current->setType(i == 0 ? Tile::TYPE_EMPTY_PLACEABLE : Tile::TYPE_EMPTY_PLACEABLE_IF_ROTATED);
         break;
      }
   }
}

void Board::draw() const
{
   /*
   gfx::Texture::disableAny();
   glBegin(GL_LINES);
   glColor3f(1,0,0); glVertex3f(0,0.1,0); glVertex3f(10, 0.1, 0);
   glColor3f(0,1,0); glVertex3f(0,0.1,0); glVertex3f(0, 10, 0);
   glColor3f(0,0,1); glVertex3f(0,0.1,0); glVertex3f(0, 0.1, 10);
   glEnd();*/

   for (auto i(board_.begin()), end(board_.end()); i != end; ++i)
   {
      if (i->second && i->second->getType() == Tile::TYPE_PLACED)
         i->second->draw();
   }
}

void Board::drawEmpyTiles() const
{
   for (auto i(empty_locations_.begin()), end(empty_locations_.end()); i != end; ++i)
   {
      Tile* tile = getTileAt(*i);
      if (tile)
         tile->draw();
   }
}

Tile* Board::makeEmpty(const glm::ivec2& position)
{
   std::unique_ptr<Tile>& ptr = board_[position];

   if (!ptr)
   {
      ptr.reset(new Tile(asset_mgr_, Tile::TYPE_EMPTY_PLACEABLE));
      ptr->setPosition(glm::vec3(position.x, 0, position.y));
      empty_locations_.insert(position);
   }

   return ptr.get();
}

} // namespace carcassonne
