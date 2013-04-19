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
#include "carcassonne/_carcassonne.h"

#include "carcassonne/features/feature.h"

namespace carcassonne {

class AssetManager;

namespace features {

class City;

class Farm : public Feature
{
public:
   Farm(AssetManager& asset_mgr, int id, Tile& tile);
   Farm(const Farm&, Tile& tile);
   virtual ~Farm();

   virtual Type getType() const;   
   virtual bool isComplete() const;
   virtual void score();
   void join(Farm& other);

   void replaceCity(const features::City& old_city, features::City& new_city);
   void addAdjacentCity(features::City& city);

private:
   std::vector<features::City*> adjacent_cities_;

   void operator=(const Farm&);
};

} // namespace carcassonne::features
} // namespace carcassonne

#endif
