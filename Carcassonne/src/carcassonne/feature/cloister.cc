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
// File: carcassonne/features/cloister.h
//
// Represents a cloister.  A cloister is only only on one tile, but scores 
// points based on the 8 neighboring tiles as well.

#include "carcassonne\features\cloister.h"

#include <algorithm>

#include "carcassonne\player.h"
#include "carcassonne\follower.h"

namespace carcassonne {
namespace features {

Cloister::Cloister(AssetManager& asset_mgr, int id, Tile& tile)
{
   follower_placeholder_.reset(new Follower(asset_mgr, id));
   tiles_.push_back(&tile);
}

Cloister::Cloister(const Cloister& other, Tile& tile)
   : Feature(other)
{
   tiles_.push_back(&tile);
}

Cloister::~Cloister()
{
}

Feature::Type Cloister::getType()const
{
   return TYPE_CLOISTER;
}

bool Cloister::isComplete()const
{
   return tiles_.size() == 9;
}

// Award tiles_.size() points to the owner of the first follower in
// followers_, if any (cloisters can only ever support one follower).
// Finally, return all followers to idle state.
void Cloister::score()
{
   int points = 0;
   points = tiles_.size();
   
   if (!followers_.empty())
   {
      Follower* f = followers_.front();
      f->getOwner()->scorePoints(points);
      f->setIdle(true);
   }
}

void Cloister::addTile(Tile& tile)
{
   auto i(std::find(tiles_.begin(), tiles_.end(), &tile));
   if (i == tiles_.end())
      tiles_.push_back(&tile);
}

} // namespace carcassonne::features
} // namespace carcassonne