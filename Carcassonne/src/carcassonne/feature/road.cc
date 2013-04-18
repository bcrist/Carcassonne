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

#include "carcassonne\player.h"
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
   int points = 0;
   points = tiles_.size();

   std::map<Player*, int> players;
   int mostFollowers= 0;
   std::vector<Player*> players_with_most_followers;

   for (auto i(followers_.begin()), end(followers_.end()); i != end; ++i)
   {
      Follower* f = *i;
      Player* owner = f->getOwner();
      f->setIdle(true);
      int& count = players[owner];

      ++count;

      if (count > mostFollowers)
      {
         mostFollowers = count;
         players_with_most_followers.clear();
         players_with_most_followers.push_back(owner);
      }
      else if (count == mostFollowers)
      {
         //they are eating bacon and dancing
         players_with_most_followers.push_back(owner);
      }
   }
   //players_with_most_followers now has all players who should receive 'points' points.

   for (auto i(players_with_most_followers.begin()), 
             end(players_with_most_followers.end()); i != end; ++i)
   {
      Player* p = *i;
      p->scorePoints(points);
   }
      
}

void Road::join(Road& other)
{
      if (this == &other)
   {
      return;
   }

   Road* survivor;
   Road* victim;
   if (tiles_.size() > other.tiles_.size())
   {
      survivor = this;
      victim = &other;
   }
   
   else 
   {
      survivor = &other;
      victim = this;
   }

   survivor->followers_.insert(survivor->followers_.end(),
                               victim->followers_.begin(),
                               victim->followers_.end());
   victim->followers_.clear();

   if (survivor->followers_.size() > 0 &&  survivor == this)
   {
      survivor->follower_placeholder_ = std::move(other.follower_placeholder_);
   }

   for (auto i(victim->tiles_.begin()), end(victim->tiles_.end()); i!= end; ++i)
   {
      Tile* t = *i;
      t->replaceRoad(*victim, *survivor);
   }
}

}
}