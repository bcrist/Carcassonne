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
// File: carcassonne/scenario.h
//
// A scenario represents the current match between some number or human or
// AI players.  "Game" might have been a better name, but our Game class
// manages the entire application, not just one match/scenario.

#include "carcassonne/scenario.h"

namespace carcassonne {

Scenario::Scenario(Game& game, std::vector<Player*>&& players)
   : game_(game),
     min_simulate_interval_(sf::milliseconds(5))
{
   
}

void Scenario::simulate(sf::Time delta)
{
   simulation_unifier_(delta);
}

void Scenario::draw()const
{
   
}

void Scenario::update()
{
   if (paused_ || clock_.getElapsedTime() < min_simulate_interval_)
      return;

   sf::Time delta = clock_.restart();
   simulate(delta);
}

bool Scenario::isPaused() const
{
   return paused_;
}

void Scenario::setPaused(bool paused)
{
   if (paused_ != paused)
   {
      paused_ = paused;

      if (paused)
         clock_.restart();
   }
}

}// namespace carcassonne