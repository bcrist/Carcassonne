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

#include <map>
#include <algorithm>

#include "carcassonne\asset_manager.h"
#include "carcassonne\db\db.h"
#include "carcassonne\db\stmt.h"
#include "carcassonne\tile.h"
#include "carcassonne\player.h"
#include "carcassonne\features\city.h"

namespace carcassonne {
namespace features {

Farm::Farm(AssetManager& asset_mgr, int id, Tile& tile)
{
   follower_placeholder_.reset(new Follower(asset_mgr, id));
   tiles_.push_back(&tile);
}

Farm::Farm(const Farm& other, Tile& tile)
   : Feature(other),
     adjacent_cities_(other.adjacent_cities_)
{
   tiles_.push_back(&tile);
}

Farm::~Farm()
{
}

Feature::Type Farm::getType()const
{
   return TYPE_FARM;
}

// return false - farms are never complete
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
   if (followers_.empty())
      return;

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
   followers_.clear();
   //players_with_most_followers now has all players who should receive 'points' points.

   for (auto i(players_with_most_followers.begin()), 
             end(players_with_most_followers.end()); i != end; ++i)
   {
      Player* p = *i;
      p->scorePoints(points);
   }
}

// merges this farm with another one (due to a tile being placed connecting
// them).
//
// If this == &other then the farms are already merged, and nothing needs
// to be done.
//
// Otherwise, the farm which covers the most tiles (or the farm passed as the
// function parameter if both farms cover the same number of tiles) will be
// designated as the surviving farm.  The smaller farm will be designated as
// the dying farm.
//
// All followers from the dying farm's followers_ vector will be placed in
// the surviving farm's followers_ vector.  If neither farm has any followers
// yet, and the surviving farm is (*this), follower_placeholder_ will be set
// to std::move(other.follower_placeholder_).  Therefore when placing a tile
// with a farm, the already placed neighbor farms should be join()ed to the
// new tile's farm, i.e. old_tile_farm.join(new_tile_farm);  (note: those are
// not real variable names).  This is accomplished by calling
// Tile::closeSide() on the existing tile before the new tile.
// 
// Finally, the dying farm will call Tile::replaceFarm() on each of the tiles
// it covers so that they now refer to the surviving farm.  This should cause
// all shared_ptrs to the dying farm to go away, and the dying farm will be
// destroyed.
void Farm::join(Farm& other)
{
   if (this == &other)
      return;

   std::shared_ptr<Feature> lock1(shared_from_this());
   std::shared_ptr<Feature> lock2(other.shared_from_this());

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

   if (survivor->followers_.empty())
   {
      if (survivor == this)
         survivor->follower_placeholder_ = std::move(other.follower_placeholder_);
   }
   else
      survivor->follower_placeholder_.reset();

   for (auto i(victim->adjacent_cities_.begin()), end(victim->adjacent_cities_.end()); i != end; ++i)
      survivor->addAdjacentCity(**i);

   victim->adjacent_cities_.clear();

   for (auto i(victim->tiles_.begin()), end(victim->tiles_.end()); i!= end; ++i)
   {
      Tile* t = *i;
      survivor->tiles_.push_back(t);
      t->replaceFarm(*victim, *survivor);
   }
   victim->tiles_.clear();
}

// called by Tile::replaceCity() when cities are joined.
void Farm::replaceCity(const features::City& old_city, features::City& new_city)
{
   auto i(std::find(adjacent_cities_.begin(), adjacent_cities_.end(), &old_city));
   if (i != adjacent_cities_.end())
      *i = &new_city;
}

void Farm::addAdjacentCity(features::City& city)
{
   auto i(std::find(adjacent_cities_.begin(), adjacent_cities_.end(), &city));
   if (i == adjacent_cities_.end())
      adjacent_cities_.push_back(&city);
}

} // namespace carcassonne::features
} // namespace carcassonne
