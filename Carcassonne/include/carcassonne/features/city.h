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

class AssetManager;

namespace features {

class City : public Feature
{
public:
   City(AssetManager& asset_mgr, int id, int pennants, Tile& tile);
   City(const City& other, Tile& tile);
   virtual ~City();

   virtual Type getType() const;
   virtual bool isComplete() const;
   virtual void score();
   void join(City& other);

private:
   int pennants_; // the number of pennants present in this city.  Each pennant
                  // provide an extra 2 points when a city is completed or an
                  // extra 1 point at the end of the game.

   City(const City&);
   void operator=(const City&);
};

} // namespace carcassonne::features
} // namespace carcassonne

#endif
