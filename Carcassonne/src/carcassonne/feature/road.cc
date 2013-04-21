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

#include <map>

#include "carcassonne\asset_manager.h"
#include "carcassonne\db\db.h"
#include "carcassonne\db\stmt.h"
#include "carcassonne\tile.h"
#include "carcassonne\player.h"

namespace carcassonne {
namespace features {
     
Road::Road(AssetManager& asset_mgr, int id, Tile& tile)
{
   db::DB& db = asset_mgr.getDB();

   db::Stmt s(db, "SELECT id "
                  "FROM cc_tile_features "
                  "WHERE id = ? AND type = ?");
   s.bind(1, id);
   s.bind(2, static_cast<int>(TYPE_ROAD));
   if (!s.step())
      throw std::runtime_error("Road not found!");

   follower_placeholder_.reset(new Follower(asset_mgr, id));
   tiles_.push_back(&tile);
}

Road::Road(const Road& other, Tile& tile)
   : Feature(other)
{
   tiles_.push_back(&tile);
}

Road::~Road()
{
}

Feature::Type Road::getType() const
{
   return TYPE_ROAD;
}

// for each TileEdge in each tile in tiles_, if TileEdge::open is true,
// TileEdge::type is TileEdge::TYPE_ROAD, and TileEdge::road is (this),
// then return false.
bool Road::isComplete() const
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

// Create a map of players to number of followers for that player, keeping
// track of the highest number of followers.  When finished iterating, find
// all players in map who have that number of followers and increase each
// player's score by tiles_.size().  Finally, return all followers to idle
// state.
void Road::score()
{
   if (followers_.empty())
      return;

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
   followers_.clear();
   //players_with_most_followers now has all players who should receive 'points' points.

   for (auto i(players_with_most_followers.begin()), 
             end(players_with_most_followers.end()); i != end; ++i)
   {
      Player* p = *i;
      p->scorePoints(points);
   }
}

// merges this road with another one (due to a tile being placed connecting
// them).
//
// If this == &other then the roads are already merged, and nothing needs
// to be done.
//
// Otherwise, the road which covers the most tiles (or the road passed as the
// function parameter if both roads cover the same number of tiles) will be
// designated as the surviving road.  The smaller road will be designated as
// the dying road.
//
// All followers from the dying road's followers_ vector will be placed in
// the surviving road's followers_ vector.  If neither road has any followers
// yet, and the surviving road is (*this), follower_placeholder_ will be set
// to std::move(other.follower_placeholder_).  Therefore when placing a tile
// with a road, the already placed neighbor roads should be join()ed to the
// new tile's road, i.e. old_tile_road.join(new_tile_road);  (note: those are
// not real variable names).  This is accomplished by calling
// Tile::closeSide() on the existing tile before the new tile.
// 
// Finally, the dying road will call Tile::replaceRoad() on each of the tiles
// it covers so that they now refer to the surviving road.  This should cause
// all shared_ptrs to the dying road to go away, and the dying road will be
// destroyed.
void Road::join(Road& other)
{
      if (this == &other)
   {
      return;
   }

   std::shared_ptr<Feature> lock1(shared_from_this());
   std::shared_ptr<Feature> lock2(other.shared_from_this());

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

   if (survivor->followers_.empty())
   {
      if (survivor == this)
         survivor->follower_placeholder_ = std::move(other.follower_placeholder_);
   }
   else
      survivor->follower_placeholder_.reset();

   for (auto i(victim->tiles_.begin()), end(victim->tiles_.end()); i!= end; ++i)
   {
      Tile* t = *i;
      survivor->tiles_.push_back(t);
      t->replaceRoad(*victim, *survivor);
   }
   victim->tiles_.clear();
}

} // namespace carcassonne::features
} // namespace carcassonne
