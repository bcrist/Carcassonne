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
// File: carcassonne/features/road.h
//
// Represents a road.  Roads are objects which can span multiple tiles.

#ifndef CARCASSONNE_FEATURES_ROAD_H_
#define CARCASSONNE_FEATURES_ROAD_H_

#include "carcassonne/features/feature.h"

namespace carcassonne {
namespace features {

class Road : public Feature
{
public:
   Road();
   virtual ~Road();

   // return TYPE_ROAD
   virtual Type getType() const;

   // for each TileEdge in each tile in tiles_, if TileEdge::open is true,
   // TileEdge::type is TileEdge::TYPE_ROAD, and TileEdge::road is (this),
   // then return false.
   virtual bool isComplete() const;

   // Create a map of players to number of followers for that player, keeping
   // track of the highest number of followers.  When finished iterating, find
   // all players in map who have that number of followers and increase each
   // player's score by tiles_.size().  Finally, return all followers to idle
   // state.
   virtual void score();

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
   void join(const Road& other);

private:

   // Disable copy-construction & assignment - do not implement
   Road(const Road&);
   void operator=(const Road&);
};

} // namespace carcassonne::features
} // namespace carcassonne

#endif
