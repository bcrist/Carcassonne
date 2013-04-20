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
// File: carcassonne/pile.h
//
// Represents a LIFO pile of tiles.  The pile can also be shuffled.

#include "carcassonne/pile.h"

#include <ctime>

#include "carcassonne/asset_manager.h"
#include "carcassonne/db/stmt.h"

namespace carcassonne {

Pile::Pile()
   : prng_(static_cast<std::mt19937::result_type>(time(nullptr)))
{
}

Pile::Pile(unsigned int seed)
   : prng_(static_cast<std::mt19937::result_type>(seed))
{
}

Pile::Pile(Pile&& other)
   : prng_(static_cast<std::mt19937::result_type>(time(nullptr)))
{
   add(std::move(other));
}

Pile& Pile::operator=(Pile&& other)
{
   tiles_ = std::move(other.tiles_);
   return *this;
}

Pile::Pile(AssetManager& asset_mgr, const std::string& tileset_name)
   : prng_(static_cast<std::mt19937::result_type>(time(nullptr)))
{
   db::DB& db = asset_mgr.getDB();

   db::Stmt s(db, "SELECT id, starting_tile FROM cc_tilesets WHERE name = ?");
   s.bind(1, tileset_name);
   if (!s.step())
      throw std::runtime_error("Tileset not found!");

   db::Stmt st(db, "SELECT tile, quantity FROM cc_tileset_tiles WHERE tileset_id = ?");
   st.bind(1, s.getInt(0));
   while (st.step())
   {
      Tile* tile = new Tile(asset_mgr, st.getText(0));
      tiles_.push_back(std::unique_ptr<Tile>(tile));

      // copy tile until quantity required has been added
      int quantity = st.getInt(1);
      while (--quantity > 0)
         tiles_.push_back(std::unique_ptr<Tile>(new Tile(*tile)));
   }

   // starting tile goes on last
   tiles_.push_back(std::unique_ptr<Tile>(new Tile(asset_mgr, s.getText(1))));
}

void Pile::setSeed()
{
   prng_.seed(static_cast<std::mt19937::result_type>(time(nullptr)));
}

void Pile::setSeed(unsigned int seed)
{
   prng_.seed(static_cast<std::mt19937::result_type>(seed));
}

void Pile::add(std::unique_ptr<Tile>&& tile)
{
   tiles_.push_back(std::move(tile));
}

void Pile::add(Pile&& other)
{
   for (auto i(other.tiles_.begin()), end(other.tiles_.end()); i != end; ++i)
   {
      tiles_.push_back(std::move(*i));
   }
   other.tiles_.clear();
}

void Pile::shuffle()
{
   for (size_t i = 0; i < tiles_.size(); ++i)
   {
      size_t j = i + prng_() % (tiles_.size() -i);
      std::swap(tiles_[i], tiles_[j]);
   }
}

std::unique_ptr<Tile> Pile::remove()
{
   if (tiles_.size() == 0)
      return std::unique_ptr<Tile>();

   std::unique_ptr<Tile> tile = std::move(tiles_.back());

   tiles_.pop_back();

   return std::move(tile);
}

} // namespace carcassonne