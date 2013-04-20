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
// File: carcassonne/features/city.h
//
// Represents a city.  Cities are objects which can span multiple tiles.

#include "carcassonne\features\city.h"

#include <map>

#include "carcassonne\asset_manager.h"
#include "carcassonne\db\db.h"
#include "carcassonne\db\stmt.h"
#include "carcassonne\tile.h"
#include "carcassonne\player.h"


namespace carcassonne {
namespace features {

City::City(AssetManager& asset_mgr, int id, int pennants, Tile& tile)
   : pennants_(pennants)
{
   follower_placeholder_.reset(new Follower(asset_mgr, id));
   tiles_.push_back(&tile);
}

City::City(const City& other, Tile& tile)
   : Feature(other),
     pennants_(other.pennants_)
{
   tiles_.push_back(&tile);
}

City::~City()
{
}

Feature::Type City::getType() const
{
   return TYPE_CITY;
}

// for each TileEdge in each tile in tiles_, if TileEdge::open is true,
// TileEdge::type is TileEdge::TYPE_CITY, and TileEdge::city is (this),
// then return false.
bool City::isComplete() const
{
   for (auto i(tiles_.begin()), end(tiles_.end()); i != end; ++i)
   {
      for (int j = Tile::SIDE_NORTH; j <= Tile::SIDE_WEST; ++j)
      {
         const TileEdge& edge = (*i)->getEdge(static_cast<Tile::Side>(j));
         if(edge.open && edge.type == TileEdge::TYPE_CITY && edge.city == this)
            return false;
      }
   }
 return true;
}

// Let 'pts' be tiles_.size() + pennants_.  If isComplete(), double 'pts'.
// Iterate through the followers in this city.  Create a map of players to
// number of followers for that player, keeping track of the highest number
// of followers.  When finished iterating, find all players in map who have
// that number of followers and increase each player's score by 'pts.'
// Finally, return all followers to idle state.
void City::score()
{
   int points = 0;
   points = tiles_.size() + pennants_;
   if (isComplete())
   {
      points *= 2;
   }

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

// merges this city with another city (due to a tile being placed connecting
// the cities).
//
// If this == &other then the cities are already merged, and nothing needs
// to be done.
//
// Otherwise, the city which covers the most tiles (or the city passed as the
// function parameter if both cities cover the same number of tiles) will be
// designated as the surviving city.  The smaller city will be designated as
// the dying city.
//
// Add the dying city's pennants_ to the surviving city's.
//
// All followers from the dying city's followers_ vector will be placed in
// the surviving city's followers_ vector.
//
// If neither city has any followers yet, and the surviving city is (*this),
// follower_placeholder_ will be set to std::move(other.follower_placeholder_).
// Therefore when placing a tile with a city, the already placed neighbor
// cities should be join()ed to the new tile's city, i.e.
// old_tile_city.join(new_tile_city);  (note: those are not real variable
// names).  This is accomplished by calling Tile::closeSide() on the existing
// tile before the new tile.
// 
// Finally, the dying city will call Tile::replaceCity() on each of the tiles
// it covers so that they now refer to the surviving city.  This should cause
// all shared_ptrs to the dying city to go away, and the dying city will be
// destroyed.
void City::join(City& other)
{
   if (this == &other)
   {
      return;
   }

   std::shared_ptr<Feature> lock1(shared_from_this());
   std::shared_ptr<Feature> lock2(other.shared_from_this());

   City* survivor;
   City* victim;
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

   survivor->pennants_ += victim->pennants_;
   victim->pennants_ = 0;

   survivor->followers_.insert(survivor->followers_.end(),
                               victim->followers_.begin(),
                               victim->followers_.end());
   victim->followers_.clear();

   if (survivor->followers_.empty() &&  survivor == this)
      survivor->follower_placeholder_ = std::move(other.follower_placeholder_);

   for (auto i(victim->tiles_.begin()), end(victim->tiles_.end()); i!= end; ++i)
   {
      Tile* t = *i;
      survivor->tiles_.push_back(t);
      t->replaceCity(*victim, *survivor);
   }
   victim->tiles_.clear();
}

} // namespace carcassonne::features
} // namespace carcassonne
