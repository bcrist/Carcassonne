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
// File: carcassonne/features/road.h
//
// Represents a road.  Roads are objects which can span multiple tiles.


#include "carcassonne\features\road.h"

#include "carcassonne\tile.h"
#include <map>

namespace carcassonne {
namespace features {
     
Road::Road()
{

}

Road::~Road(){}

Feature::Type Road::getType()const
{
   return TYPE_ROAD;
}

bool Road::isComplete()const
{
   for (auto i(tiles_.begin()), end(tiles_.end()); i != end; ++i)
   {
      for (int j = Tile::SIDE_NORTH; j <= Tile::SIDE_WEST; ++j)
      {
         const TileEdge& edge = (*i)->getEdge(static_cast<Tile::Side>(j));
         if(edge.open && edge.type == TileEdge::TYPE_ROAD && edge.road == this)
            return false;
      }
   }
 return true;
}

void Road::score()
{
   std::map<Player*, int> players;

   for (auto i(followers_.begin()), end(followers_.end()); i != end; ++i)
   {
      Follower* f = *i;
      
      ++players[f->getOwner()];
   }
}

void Road::join(const Road& other)
{

}

}
}