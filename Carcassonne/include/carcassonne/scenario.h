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
// File: carcassonne/scenario.h
//
// A scenario represents the current match between some number or human or
// AI players.  "Game" might have been a better name, but our Game class
// manages the entire application, not just one match/scenario.

#ifndef CARCASSONNE_SCENARIO_H_
#define CARCASSONNE_SCENARIO_H_
#include "carcassonne/_carcassonne.h"

#include <vector>
#include <SFML/System.hpp>

#include "carcassonne/board.h"
#include "carcassonne/pile.h"
#include "carcassonne/player.h"

namespace carcassonne {

class Scenario
{
public:

   enum Phase {
      PHASE_TILE_PLACEMENT = 0,
      PHASE_FOLLOWER_PLACEMENT = 1
   };

   Scenario(std::vector<Player*>&& players);

   void simulate(sf::Time delta);

   void draw() const;


private:
   Board board_;
   Pile draw_pile_;
   std::vector<Player*> players_;

   std::vector<Player*>::iterator current_player_; // iterator to the player
                                                   // whose turn it is.
   Phase current_phase_;                // the phase of the current player's turn
   std::unique_ptr<Tile> current_tile_; // the tile that is currently being played

   // Disable copy-construction & assignment - do not implement
   Scenario(const Scenario&);
   void operator=(const Scenario&);
};

} // namespace carcassonne

#endif
