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
// File: carcassonne/pile.h
//
// Represents a LIFO pile of tiles.  The pile can also be shuffled.

#ifndef CARCASSONNE_PILE_H_
#define CARCASSONNE_PILE_H_
#include "carcassonne/_carcassonne.h"

#include <vector>
#include <memory>
#include <random>

#include "carcassonne/tile.h"

namespace carcassonne {

class Pile
{
public:
   Pile(); //
   Pile(unsigned int seed);

   void setSeed(unsigned int seed);

   // assert that tile's type is TYPE_FLOATING
   void add(std::unique_ptr<Tile>&& tile);

   // move all tiles from other to the top of this pile.
   void add(Pile&& other);

   void shuffle();

   std::unique_ptr<Tile> remove();

private:
   std::mt19937 prng_; // PRNG => psudo-random number generator
   std::vector<std::unique_ptr<Tile> > tiles_;

   // Disable copy-construction & assignment - do not implement
   Pile(const Pile&);
   void operator=(const Pile&);
};

} // namespace carcassonne

#endif
