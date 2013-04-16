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
// Author: Benjamin Crist
// File: carcassonne/features/farm.h
//
// Represents a farm.  Farms are objects which can span multiple tiles.
// They are scored only at the end of the game, based on the number of
// completed cities which they border.

#ifndef CARCASSONNE_FEATURES_FARM_H_
#define CARCASSONNE_FEATURES_FARM_H_

#include "carcassonne/features/feature.h"
#include "carcassonne/features/city.h"
namespace carcassonne {
namespace features {

class Farm : public Feature
{
public:
   Farm();
   virtual ~Farm();

   // return TYPE_FARM
   virtual Type getType() const;

   // return false - farms are never complete
   virtual bool isComplete() const;

   // Remove all incomplete cities from adjacent_cities_.  Create a map of
   // players to number of followers for that player, keeping track of the
   // highest number of followers.  When finished iterating, find all players
   // in map who have that number of followers and increase each player's score
   // adjacent_cities_.size() * 3.
   // Finally, return all followers to idle state.
   virtual void score();

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
   void join(const Farm& other);

   // called by Tile::replaceCity() when cities are joined.
   void replaceCity(const features::City& old_city, const features::City& new_city);


private:
   std::vector<features::City*> adjacent_cities_;


   // Disable copy-construction & assignment - do not implement
   Farm(const Farm&);
   void operator=(const Farm&);
};

} // namespace carcassonne::features
} // namespace carcassonne

#endif
