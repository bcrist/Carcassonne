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
#include "carcassonne/gui/input_manager.h"
#include "carcassonne/scheduling/Unifier.h"
#include "carcassonne/scheduling/persistent_sequence.h"

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

   void placeTile(const glm::ivec2& board_coords);
   void placeFollower(const glm::vec3& world_coords);
   void endTurn();

   void zoom(float factor, bool lock_xz);

   void draw() const;
   void update();

   bool isPaused() const;
   void setPaused(bool paused);

   void simulate(sf::Time delta);

   void onHover();
   void onLeftDown();
   void onRightDown();
   void onLeftDrag(const glm::ivec2& down_position);
   void onRightDrag(const glm::ivec2& down_position);
   void onLeftUp(const glm::ivec2& down_position);
   void onRightUp(const glm::ivec2& down_position);
   void onLeftCancel(const glm::ivec2& down_position);
   void onRightCancel(const glm::ivec2& down_position);

   void onKey(const sf::Event::KeyEvent& event, bool down);
   void onCharacter(const sf::Event::TextEvent& event);

   void onMouseMoved(const glm::ivec2& window_coords);
   void onMouseWheel(int delta);
   void onMouseButton(sf::Mouse::Button button, bool down);

   void onResized();
   void onBlurred();
   bool onClosed();
   void cancelInput();

private:
   Game& game_;

   gui::InputManager input_mgr_;

   gfx::PerspectiveCamera camera_;
   gfx::OrthoCamera hud_camera_;
   float floating_height_;
   bool camera_movement_enabled_;

   sf::Clock clock_;
   sf::Time min_simulate_interval_;
   bool paused_;
   scheduling::Unifier simulation_unifier_;
   scheduling::PersistentSequence simulation_sequence_;
   
   Board board_;
   Pile draw_pile_;
   std::vector<Player*> players_;

   std::vector<Player*>::iterator current_player_; // iterator to the player
                                                   // whose turn it is
   std::unique_ptr<Tile> current_tile_; // the tile that is currently being played
   Tile* last_placed_tile_;
   Follower* current_follower_;
   std::vector<std::shared_ptr<features::Feature> > follower_placeholders_;

   // Disable copy-construction & assignment - do not implement
   Scenario(const Scenario&);
   void operator=(const Scenario&);
};

} // namespace carcassonne

#endif
