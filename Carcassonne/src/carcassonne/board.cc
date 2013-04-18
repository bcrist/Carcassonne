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

namespace carcassonne {

Board::Board()
{
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
         Tile* old_tile = getTileAt(position);
         if (old_tile == nullptr || old_tile->getType() != Tile::TYPE_EMPTY_PLACEABLE)
            return false;

         board_[position] = std::move(tile);
         return true;

      default:
         return false;
   }
}

void Board::draw() const
{
}

} // namespace carcassonne
