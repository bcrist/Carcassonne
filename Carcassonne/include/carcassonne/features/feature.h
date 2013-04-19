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
// File: carcassonne/features/feature.h
//
// Base class for cities, cloisters, farms, and roads.

#ifndef CARCASSONNE_FEATURES_FEATURE_H_
#define CARCASSONNE_FEATURES_FEATURE_H_

#include <vector>

#include "carcassonne/follower.h"

namespace carcassonne {

class Tile;

namespace features {

class Feature : public std::enable_shared_from_this<Feature>
{
public:

   enum Type {
      TYPE_FARM = 0,
      TYPE_ROAD = 1,
      TYPE_CITY = 2,
      TYPE_CLOISTER = 3
   };

   Feature();
   virtual ~Feature(); 

   virtual Type getType() const = 0; // returns the type of feature this is.
   bool isCity() const;          // If Feature* pointers are used, this allows
   bool isCloister() const;      // them to be reliably converted to the proper
   bool isFarm() const;          // type using static_cast (avoiding use of RTTI)
   bool isRoad() const;

   // returns true if this feature is complete, and can be scored immediately.
   // After each turn, this is called on each feature of the placed tile, and
   // if it returns true, the feature's score() function is called.
   virtual bool isComplete() const = 0;

   // Called when a feature becomes complete and at the end of the game to
   // award points to the player(s) who have the most followers in/on the
   // feature.  After scoring, the feature's followers are returned to
   // idle state.
   virtual void score() = 0;

   void placeFollower(Follower& follower);
      

protected:
   // The tiles which contain at least a part of this feature.  For cloisters,
   // this also contains up to 8 tiles which border the cloister.
   std::vector<Tile*> tiles_;

   // A follower not owned by any player indicating the location where a
   // player can deploy a follower.  If this feature already has one or more
   // followers, this should be reset to null.
   std::unique_ptr<Follower> follower_placeholder_;  

   // The followers that currently inhabit this feature.  (Does not include
   // placeholder followers)
   std::vector<Follower*> followers_;  

   // Disable copy-construction & assignment - do not implement
   Feature(const Feature&);
   void operator=(const Feature&);
};

} // namespace carcassonne::features
} // namespace carcassonne

#endif
