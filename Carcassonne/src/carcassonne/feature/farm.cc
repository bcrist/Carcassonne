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
// File: carcassonne/features/farm.h
//
// Represents a farm.  Farms are objects which can span multiple tiles.
// They are scored only at the end of the game, based on the number of
// completed cities which they border.

#include "carcassonne\features\farm.h"
#include "carcassonne\tile.h"
#include "carcassonne\player.h"

#include <map>

namespace carcassonne {
namespace features {

Farm::Farm()
{
   
}

Farm::~Farm(){}

Feature::Type Farm::getType()const
{
   return TYPE_FARM;
}

bool Farm::isComplete()const
{
   return false;
}

// Remove all incomplete cities from adjacent_cities_.  Create a map of
// players to number of followers for that player, keeping track of the
// highest number of followers.  When finished iterating, find all players
// in map who have that number of followers and increase each player's score
// adjacent_cities_.size() * 3.
// Finally, return all followers to idle state.
void Farm::score()
{
   std::vector<features::City*> complete_cities;
   for (auto i(adjacent_cities_.begin()), end(adjacent_cities_.end()); i !=end; ++i)
   {
      City* c = *i;
      if (c->isComplete())
      {
         complete_cities.push_back(c);
      }
   }
   adjacent_cities_.clear();

   int points = 3 * complete_cities.size();

   std::map<Player*, int> players;
   int mostFollowers = 0;
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

void Farm::join(Farm& other)
{
   if (this == &other)
   {
      return;
   }

   Farm* survivor;
   Farm* victim;
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
      t->replaceFarm(*victim, *survivor);
   }

}

}
}