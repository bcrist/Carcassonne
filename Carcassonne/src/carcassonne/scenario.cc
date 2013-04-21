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
     board_(game.getAssetManager()),
     draw_pile_(std::move(options.tiles)),
     players_(options.players),
     current_player_(players_.end()),
     current_follower_(nullptr),
     last_placed_tile_(nullptr)
{
   camera_.setPosition(glm::vec3(-4, 6, -1));
   camera_.setTarget(glm::vec3(0, 0, 0));
   camera_.recalculatePerspective();
   hud_camera_.recalculate();

   assert(players_.size() > 1);

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

// switches to follower placement
void Scenario::placeTile(const glm::ivec2& board_coords)
{
   last_placed_tile_ = current_tile_.get();
   if (!board_.placeTileAt(board_coords, std::move(current_tile_)))
   {
      last_placed_tile_ = nullptr;
      return;
   }
   current_tile_.reset();

   // TODO: switch to follower placement first
   Follower* follower = getCurrentPlayer().getIdleFollower();
   if (follower != nullptr)
   {
      bool found_placeholder(false);
      for (size_t i = 0; i < last_placed_tile_->getFeatureCount(); ++i)
      {
         std::shared_ptr<features::Feature> feature = last_placed_tile_->getFeature(i).lock();
         if (feature->hasPlaceholder())
         {
            found_placeholder = true;
            glm::vec4 color(follower->getOwner()->getColor());
            feature->setPlaceholderColor(color);
            follower_placeholders_.push_back(feature);
         }
      }
      if (found_placeholder)
      {
         current_follower_ = follower;
         return;  // don't end turn
      }
   }

   endTurn();
}

void Scenario::placeFollower(const glm::vec3& world_coords)
{
   glm::vec3& tile_coords(last_placed_tile_->worldToLocal(world_coords));

   features::Feature* closest_feature(nullptr);
   float closest_placeholder_distance(1.0f);
   for (auto i(follower_placeholders_.begin()), end(follower_placeholders_.end()); i != end; ++i)
   {
      features::Feature* feature = i->get();
      const Follower* follower = feature->getPlaceholder();
      float distance(glm::distance(tile_coords, follower->getPosition()));

      if (closest_placeholder_distance > distance)
      {
         closest_placeholder_distance = distance;
         closest_feature = feature;
      }
   }

   if (closest_feature)
      closest_feature->placeFollower(*current_follower_, *last_placed_tile_);
   
   endTurn();
}

void Scenario::endTurn()
{
   if (last_placed_tile_)
      last_placed_tile_->checkForCompleteFeatures();

   // move current_player_ to the next player
   if (current_player_ == players_.end())
      current_player_ = players_.begin();
   else
   {
      ++current_player_;
      if (current_player_ == players_.end())
         current_player_ = players_.begin();
   }

   // set current_tile_
   current_tile_ = draw_pile_.remove();

   // TODO: check for end of pile (game over)

   if (current_tile_)
      board_.usingNewTile(*current_tile_);

   // set current_follower_
   current_follower_ = nullptr;
   last_placed_tile_ = nullptr;
   follower_placeholders_.clear();
}

void Scenario::onMouseMoved(const glm::ivec2& window_coords)
{
   if (paused_)
      return;

   mouse_position_ = window_coords;

   onMousePositionChanged();
}

void Scenario::onMouseWheel(int delta)
{
   if (paused_)
      return;

   glm::vec3 look_direction(camera_.getTarget() - camera_.getPosition());

   float look_length = glm::length(look_direction);

   float factor = 0.85f;
   
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
   if (paused_)
      return;

   if (button == sf::Mouse::Left && down == false && getCurrentPlayer().isHuman())
   {
      if (current_tile_)
      {
         glm::vec3 world_coords(camera_.windowToWorld(glm::vec2(mouse_position_), floating_height_));
         glm::ivec2 board_coords(board_.getCoordinates(world_coords));

         placeTile(board_coords);
      }
      else if (current_follower_)
      {
         glm::vec3 world_coords(camera_.windowToWorld(glm::vec2(mouse_position_), 0.1f));

         placeFollower(world_coords);
      }
   }

   else if (button == sf::Mouse::Right && down == false && getCurrentPlayer().isHuman())
   {
      if (current_tile_)
      {
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
             sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
         {
            current_tile_->rotateCounterclockwise();
         }
         else
         {
            current_tile_->rotateClockwise();
         }
         board_.tileRotated(*current_tile_);
      }
   }
}

void Scenario::onKey(const sf::Event::KeyEvent& event, bool down)
{
   if (paused_)
      return;
}

void Scenario::onCharacter(const sf::Event::TextEvent& event)
{
   if (paused_)
      return;
}

void Scenario::onResized()
{
   camera_.recalculatePerspective();
   hud_camera_.recalculate();
}

void Scenario::onBlurred()
{
   cancelInput();
   if (!paused_)
      game_.pushMenu("pause");
}

bool Scenario::onClosed()
{
   return true;
}

void Scenario::cancelInput()
{
}

void Scenario::onCameraMoved()
{
   onMousePositionChanged();
}

void Scenario::onMousePositionChanged()
{
   if (getCurrentPlayer().isHuman())
   {
      glm::vec3 world_coords(camera_.windowToWorld(glm::vec2(mouse_position_), floating_height_));

      if (current_follower_)
      {
         current_follower_->setPosition(world_coords);
      }
      else if (current_tile_)
      {
         current_tile_->setPosition(world_coords);
      }
   }
}

void Scenario::draw() const
{
   camera_.use();

   glEnable(GL_LIGHTING);

   board_.draw();
   if (getCurrentPlayer().isHuman() && current_tile_)
      board_.drawEmpyTiles();

   if (current_follower_)
   {
      last_placed_tile_->drawPlaceholders();
      current_follower_->draw();
   }
   else if (current_tile_)
      current_tile_->draw();

   for (auto i(players_.begin()), end(players_.end()); i != end; ++i)
   {
      Player& p = **i;
      p.drawPlacedFollowers();
   }


   glDisable(GL_LIGHTING);

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
         cancelInput();
      else
         clock_.restart();
   }
}

void Scenario::simulate(sf::Time delta)
{
   simulation_unifier_(delta);
}

}// namespace carcassonne