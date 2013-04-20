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
#include <SFML/Window.hpp>

#include "carcassonne/board.h"
#include "carcassonne/pile.h"
#include "carcassonne/player.h"
#include "carcassonne/gfx/perspective_camera.h"
#include "carcassonne/gfx/ortho_camera.h"
#include "carcassonne/scheduling/Unifier.h"

namespace carcassonne {

class Game;

struct ScenarioInit
{
   std::vector<Player*> players;
   Pile tiles;
   std::unique_ptr<Tile> starting_tile;
};

class Scenario
{
public:
   Scenario(Game& game, ScenarioInit& options);

   Player& getCurrentPlayer();
   const Player& getCurrentPlayer() const;

   void placeTile();
   void endTurn();

   void onMouseMoved(const glm::ivec2& window_coords);
   void onMouseWheel(int delta);
   void onMouseButton(sf::Mouse::Button button, bool down);

   void onKey(const sf::Event::KeyEvent& event, bool down);
   void onCharacter(const sf::Event::TextEvent& event);

   void onResized();
   void onBlurred();
   bool onClosed();

   void draw() const;
   void update();

   bool isPaused() const;
   void setPaused(bool paused);

   void simulate(sf::Time delta);

private:
   Game& game_;

   gfx::PerspectiveCamera camera_;
   gfx::OrthoCamera hud_camera_;
   glm::ivec2 mouse_position_;
   float floating_height_;

   sf::Clock clock_;
   sf::Time min_simulate_interval_;
   bool paused_;
   scheduling::Unifier simulation_unifier_;
   
   Board board_;
   Pile draw_pile_;
   std::vector<Player*> players_;

   std::vector<Player*>::iterator current_player_; // iterator to the player
                                                   // whose turn it is
   std::unique_ptr<Tile> current_tile_; // the tile that is currently being played
   Follower* current_follower_;

   // Disable copy-construction & assignment - do not implement
   Scenario(const Scenario&);
   void operator=(const Scenario&);
};

} // namespace carcassonne

#endif
