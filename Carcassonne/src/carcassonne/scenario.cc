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

#include "carcassonne/pile.h"
#include "carcassonne/game.h"

namespace carcassonne {

Scenario::Scenario(Game& game, ScenarioInit& options)
   : game_(game),
     camera_(game.getGraphicsConfiguration()),
     hud_camera_(game.getGraphicsConfiguration()),
     floating_height_(0.5f),
     min_simulate_interval_(sf::milliseconds(5)),
     paused_(false),
     players_(options.players),
     current_player_(players_.end()),
     current_follower_(nullptr)
{
   camera_.setPosition(glm::vec3(-2, 10, -4));
   camera_.setTarget(glm::vec3(0, 0, 0));
   assert(players_.size() > 1);

   draw_pile_.add(std::move(options.tiles));

   board_.placeTileAt(glm::ivec2(0,0), std::move(options.starting_tile));
   endTurn();
}

Player& Scenario::getCurrentPlayer()
{
   return **current_player_;
}

const Player& Scenario::getCurrentPlayer() const
{
   return **current_player_;
}

void Scenario::endTurn()
{
   // move current_player_ to the next player
   if (current_player_ == players_.end())
      current_player_ = players_.begin();
   else
      ++current_player_;

   // set current_tile_
   current_tile_ = draw_pile_.remove();

   // set current_follower_
   current_follower_ = nullptr;
}

void Scenario::onMouseMoved(const glm::ivec2& window_coords)
{
   if (getCurrentPlayer().isHuman())
   {
      glm::vec3 world_coords(camera_.windowToWorld(glm::vec2(window_coords), floating_height_));

      if (current_tile_)
      {
         current_tile_->setPosition(world_coords);
      }
      else if (current_follower_)
      {
         current_follower_->setPosition(world_coords);
      }
   }
}

void Scenario::onMouseWheel(int delta)
{
   glm::vec3 look_direction(camera_.getTarget() - camera_.getPosition());

   float look_length = glm::length(look_direction);

   float factor = 1.5f;
   
   if (delta < 0)
   {
      delta = -delta;
      factor = 1 / factor;
   }

   while (--delta > 0)
      factor *= factor;

   look_length *= factor;

   if (look_length < 3 || look_length > 150) // TODO: set min/max look length from graphicsconfig
      return;

   camera_.setPosition(camera_.getTarget() - look_direction * factor);
}

void Scenario::onMouseButton(sf::Mouse::Button button, bool down)
{
}

void Scenario::onKey(const sf::Event::KeyEvent& event, bool down)
{
}

void Scenario::onCharacter(const sf::Event::TextEvent& event)
{
}

void Scenario::onResized()
{
   camera_.recalculatePerspective();
   hud_camera_.recalculate();
}

void Scenario::onBlurred()
{
}

bool Scenario::onClosed()
{
   return false;
}

void Scenario::draw() const
{
   camera_.use();
   board_.draw();

   if (current_tile_)
      current_tile_->draw();
   else if (current_follower_)
      current_follower_->draw();


   hud_camera_.use();
   // draw current player's HUD
   getCurrentPlayer().draw();

   // TODO: draw all players' scores
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

void Scenario::simulate(sf::Time delta)
{
   simulation_unifier_(delta);
}

}// namespace carcassonne