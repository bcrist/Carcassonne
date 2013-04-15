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
// File: carcassonne/features/city.h
//
// Represents a city.  Cities are objects which can span multiple tiles.

#ifndef CARCASSONNE_FEATURES_CITY_H_
#define CARCASSONNE_FEATURES_CITY_H_

#include "carcassonne/features/feature.h"

namespace carcassonne {
namespace features {

class City : public Feature
{
public:
   City();
   virtual ~City();

   // return TYPE_CITY
   virtual Type getType() const;

   // for each TileEdge in each tile in tiles_, if TileEdge::open is true,
   // TileEdge::type is TileEdge::TYPE_CITY, and TileEdge::city is (this),
   // then return false.
   virtual bool isComplete() const;

   // Let 'pts' be tiles_.size() + pennants_.  If isComplete(), double 'pts'.
   // Iterate through the followers in this city.  Create a map of players to
   // number of followers for that player, keeping track of the highest number
   // of followers.  When finished iterating, find all players in map who have
   // that number of followers and increase each player's score by 'pts.'
   // Finally, return all followers to idle state.
   virtual void score();

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
   // the surviving city's followers_ vector.  If neither city has any followers
   // yet, and the surviving city is (*this), follower_placeholder_ will be set
   // to std::move(other.follower_placeholder_).  Therefore when placing a tile
   // with a city, the already placed neighbor cities should be join()ed to the
   // new tile's city, i.e. old_tile_city.join(new_tile_city);  (note: those are
   // not real variable names).  This is accomplished by calling
   // Tile::closeSide() on the existing tile before the new tile.
   // 
   // Finally, the dying city will call Tile::replaceCity() on each of the tiles
   // it covers so that they now refer to the surviving city.  This should cause
   // all shared_ptrs to the dying city to go away, and the dying city will be
   // destroyed.
   void join(const City& other);

private:
   int pennants_; // the number of pennants present in this city.  Each pennant
                  // provide an extra 2 points when a city is completed or an
                  // extra 1 point at the end of the game.

   // Disable copy-construction & assignment - do not implement
   City(const City&);
   void operator=(const City&);
};

} // namespace carcassonne::features
} // namespace carcassonne

#endif
