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

#include <sstream>

#include "carcassonne/pile.h"
#include "carcassonne/game.h"
#include "carcassonne/scheduling/interpolator.h"
#include "carcassonne/scheduling/method.h"
#include "carcassonne/scheduling/easing/easings.h"

namespace carcassonne {

Scenario::Scenario(Game& game, ScenarioInit& options)
   : game_(game),
     camera_(game.getGraphicsConfiguration()),
     hud_camera_(game.getGraphicsConfiguration()),
     floating_height_(0.5f),
     camera_movement_enabled_(true),
     font_(game.getAssetManager().getTextureFont("kingthings")),
     min_simulate_interval_(sf::milliseconds(5)),
     paused_(false),
     board_(game.getAssetManager()),
     draw_pile_(std::move(options.tiles)),
     players_(options.players),
     current_player_(players_.end()),
     current_follower_(nullptr),
     last_placed_tile_(nullptr),
     game_over_(false)
{
   // set InputManager callbacks
   input_mgr_.setMouseHoverHandler(                   ([=](){ onHover(); }));
   input_mgr_.setMouseDownHandler(sf::Mouse::Left,    ([=](){ onLeftDown(); }));
   input_mgr_.setMouseDownHandler(sf::Mouse::Right,   ([=](){ onRightDown(); }));
   input_mgr_.setMouseDragHandler(sf::Mouse::Left,    ([=](const glm::ivec2& pos){ onLeftDrag(pos); }));
   input_mgr_.setMouseDragHandler(sf::Mouse::Right,   ([=](const glm::ivec2& pos){ onRightDrag(pos); }));
   input_mgr_.setMouseUpHandler(sf::Mouse::Left,      ([=](const glm::ivec2& pos){ onLeftUp(pos); }));
   input_mgr_.setMouseUpHandler(sf::Mouse::Right,     ([=](const glm::ivec2& pos){ onRightUp(pos); }));
   input_mgr_.setMouseCancelHandler(sf::Mouse::Left,  ([=](const glm::ivec2& pos){ onLeftCancel(pos); }));
   input_mgr_.setMouseCancelHandler(sf::Mouse::Right, ([=](const glm::ivec2& pos){ onRightCancel(pos); }));

   camera_.setPosition(glm::vec3(-4, 6, -1));
   camera_.setTarget(glm::vec3(0, 0, 0));
   camera_.recalculatePerspective();

   hud_camera_.setClient(gfx::Rect(0.0f, 0.0f, 1.33f, 1.0f));
   hud_camera_.recalculate();

   scheduling::PersistentSequence* seq = &simulation_sequence_;
   simulation_unifier_.schedule([=] (sf::Time t) { return (*seq)(t); });

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
         follower->setFloating(true);
         onHover();
         return;  // don't end turn
      }
   }

   endTurn();
}

void Scenario::placeFollower(const glm::vec3& world_coords, bool limit_distance)
{
   glm::vec3& tile_coords(last_placed_tile_->worldToLocal(world_coords));

   features::Feature* closest_feature(nullptr);
   float closest_placeholder_distance(limit_distance ? 1.0f : -1.0f);
   for (auto i(follower_placeholders_.begin()), end(follower_placeholders_.end()); i != end; ++i)
   {
      features::Feature* feature = i->get();
      const Follower* follower = feature->getPlaceholder();
      float distance(glm::distance(tile_coords, follower->getPosition()));

      if (closest_placeholder_distance < 0 || closest_placeholder_distance > distance)
      {
         closest_placeholder_distance = distance;
         closest_feature = feature;
      }
   }

   if (closest_feature)
      closest_feature->placeFollower(*current_follower_, *last_placed_tile_);
   else
      current_follower_->setIdle(true);
   
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
   camera_movement_enabled_ = (**current_player_).isHuman();

   // set current_tile_
   current_tile_ = draw_pile_.remove();

   while (current_tile_)
   {
      if (board_.usingNewTile(*current_tile_))
         break;

      // if the current tile isn't placeable anywhere, get a new tile and discard the old one
      current_tile_ = draw_pile_.remove();
   }

   if (!current_tile_)
   {
      board_.scoreAllTiles();

      std::cout << "Game Over!" <<std::endl;
      int p = 1;
      for (auto i(players_.begin()), end(players_.end()); i != end; ++i)
      {
         std::cout << "Player " << p << ": " << (*i)->getScore() << " points" << std::endl;
         ++p;
      }
      game_over_ = true;
   }

   

   // set current_follower_
   current_follower_ = nullptr;
   last_placed_tile_ = nullptr;
   follower_placeholders_.clear();

   onHover();
}


void Scenario::zoom(float factor, bool lock_xz)
{
   if (!camera_movement_enabled_)
      return;

   glm::vec3 look_direction(camera_.getTarget() - camera_.getPosition());

   if (lock_xz)
      look_direction.y *= factor;
   else
      look_direction *= factor;

   float look_length = glm::length(look_direction);

   if (factor < 1 && look_length < 3 ||
       factor > 1 && look_length > 150 ||
       factor < 1 && abs(look_direction.y) < 1.0f) // TODO: set min/max look length from graphicsconfig
   {
      return;
   }

   camera_.setPosition(camera_.getTarget() - look_direction);
   onHover();
}


void Scenario::draw() const
{
   camera_.use();

   glEnable(GL_LIGHTING);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   
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
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);
   hud_camera_.use();

   const gfx::Rect& expanded = hud_camera_.getExpandedClientRect();

   // draw tiles remaining count
   glPushMatrix();
   glTranslatef(expanded.right(), 0.0f, 0.0f);
   glScalef(0.3f, 0.3f, 0.3f);

   std::ostringstream oss;
   oss << draw_pile_.size() << " tiles remain";
   float w = font_->getWidth(oss.str());
   glTranslatef(-(w + 0.1f), 0.15f, 0.0f);
   glColor4f(1,1,1,0.5);
   font_->print(oss.str(), GL_MODULATE);

   glPopMatrix();

   glPushMatrix();
   glTranslatef(expanded.left(), 0, 0);

   // draw current player's HUD
   getCurrentPlayer().draw();

   // draw all players' scores
   glScalef(0.3f, 0.3f, 0.3f);
   glTranslatef(0.1f, 0.15f, 0.0f);

   for (auto i(players_.begin()), end(players_.end()); i != end; ++i)
   {
      bool active = &getCurrentPlayer() == *i;

      std::ostringstream oss;
      oss << (*i)->getName() << ": " << (*i)->getScore() << " points";

      glm::vec4 color((*i)->getColor());
      if (active)
      {
         glTranslatef(0.0f, 0.05f, 0.0f);
         glScalef(1.5f, 1.5f, 1.5f);
      }
      else
      {
         color.a = 0.5f;
      }

      glColor4fv(glm::value_ptr(color));
      font_->print(oss.str(), GL_MODULATE);

      if (active)
         glScalef(0.75f, 0.75f, 0.75f);

      glTranslatef(0.0f, 0.15f, 0.0f);
   }

   glPopMatrix();

   glEnable(GL_DEPTH_TEST);
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
   if (!getCurrentPlayer().isHuman() && !game_over_)
   {
      if (current_tile_)
      {
         const glm::ivec2* location = board_.getNextPlaceableLocation();
         
         while (location == nullptr)
         {
            current_tile_->rotateClockwise();
            board_.tileRotated(*current_tile_);
            location = board_.getNextPlaceableLocation();
         }

         glm::ivec2 loc = *location;
         placeTile(loc);
      }
      else if (current_follower_)
      {
         placeFollower(glm::vec3(), false);
      }
      else
         endTurn();
   }


   if (camera_movement_enabled_)
   {
      bool w_down = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
      bool s_down = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
      bool a_down = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
      bool d_down = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
      bool q_down = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
      bool e_down = sf::Keyboard::isKeyPressed(sf::Keyboard::E);

      if (w_down ^ s_down)
      {
         glm::vec3 direction = camera_.getTarget() - camera_.getPosition();
         float length = glm::length(direction);
         direction.y = 0;
         direction = glm::normalize(direction) * (delta.asSeconds() * length * 0.5f);

         if (w_down)
            camera_.translate(direction);
         else
            camera_.translate(-direction);

         onHover();
      }

      if (a_down ^ d_down)
      {
         glm::vec3 direction = camera_.getTarget() - camera_.getPosition();
         float length = glm::length(direction);
         direction.y = 0;
         direction = glm::normalize(direction) * (delta.asSeconds() * length * 0.5f);
         direction = glm::cross(direction, camera_.getUp());

         if (a_down)
            camera_.translate(-direction);
         else
            camera_.translate(direction);

         onHover();
      }

      if (q_down ^ e_down)
      {
         float angle = delta.asSeconds() * 75.0f;
         if (q_down)
            camera_.rotateAroundTarget(angle);
         else
            camera_.rotateAroundTarget(-angle);

         onHover();
      }
   }

   simulation_unifier_(delta);
}



void Scenario::onHover()
{
   if (getCurrentPlayer().isHuman())
   {
      glm::vec3 world_coords(camera_.windowToWorld(glm::vec2(input_mgr_.getMousePosition()), floating_height_));

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

void Scenario::onLeftDown()
{
}

void Scenario::onRightDown()
{
}

void Scenario::onLeftDrag(const glm::ivec2& down_position)
{
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && camera_movement_enabled_)
   {
      glm::vec3 new_position = camera_.windowToWorld(glm::vec2(input_mgr_.getMousePosition()), 0);
      glm::vec3 old_position = camera_.windowToWorld(glm::vec2(input_mgr_.getMousePosition() - input_mgr_.getMouseDelta()), 0);

      glm::vec3 delta = old_position - new_position;

      camera_.translate(delta);
      //onHover();
   }
}

void Scenario::onRightDrag(const glm::ivec2& down_position)
{
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && camera_movement_enabled_)
   {
      glm::vec2 delta = glm::vec2(input_mgr_.getMouseDelta());
      camera_.rotateAroundTarget(delta.x * 0.4f);
   }
}

void Scenario::onLeftUp(const glm::ivec2& down_position)
{
   if (game_over_)
      game_.pushMenu("splash");

   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
   {
      onHover();
   }
   else if (getCurrentPlayer().isHuman())
   {
      if (current_tile_)
      {
         glm::vec3 world_coords(camera_.windowToWorld(glm::vec2(input_mgr_.getMousePosition()), floating_height_));
         glm::ivec2 board_coords(board_.getCoordinates(world_coords));

         placeTile(board_coords);
      }
      else if (current_follower_)
      {
         glm::vec3 world_coords(camera_.windowToWorld(glm::vec2(input_mgr_.getMousePosition()), 0.1f));

         placeFollower(world_coords, true);
      }
   }
}

void Scenario::onRightUp(const glm::ivec2& down_position)
{
   if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
   {
      onHover();
   }
   else if (getCurrentPlayer().isHuman())
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

void Scenario::onLeftCancel(const glm::ivec2& down_position)
{
}

void Scenario::onRightCancel(const glm::ivec2& down_position)
{
}


void Scenario::onKey(const sf::Event::KeyEvent& event, bool down)
{
   if (paused_)
      return;

   if (down == false)
   {
      switch (event.code)
      {
         case sf::Keyboard::Escape:
            setPaused(true);
            //game_.close();
            //game_.pushMenu("paused");
            break;

         case sf::Keyboard::Tab:
            if (camera_movement_enabled_ && current_tile_)
            {
               const glm::ivec2* location = board_.getNextPlaceableLocation();

               if (location)
               {
                  simulation_sequence_.clear();

                  gfx::PerspectiveCamera* camera = &camera_;
                  bool* camera_movement_enabled = &camera_movement_enabled_;
                  glm::vec3 camera_position_offset = camera_.getPosition() - camera_.getTarget();

                  //simulation_sequence_.schedule(scheduling::Method<>([=](){ *camera_movement_enabled = false; }));

                  simulation_sequence_.schedule(scheduling::Interpolator<glm::vec3, const glm::vec3&>(
                     sf::seconds(0.5), ([=](const glm::vec3& position) {
                        camera->setTarget(position);
                        camera->setPosition(position + camera_position_offset);
                        onHover();
                     }), camera_.getTarget(), glm::vec3(location->x, 0, location->y),
                     scheduling::easing::QuadraticOut()));

                  //simulation_sequence_.schedule(scheduling::Method<>([=](){ *camera_movement_enabled = getCurrentPlayer().isHuman(); }));
               }
            }
            break;

         default:
            break;
      }
   }

   input_mgr_.onKey(event, down);
}

void Scenario::onCharacter(const sf::Event::TextEvent& event)
{
   if (paused_)
      return;
}

void Scenario::onMouseMoved(const glm::ivec2& window_coords)
{
   if (paused_)
      return;

   input_mgr_.onMouseMoved(window_coords);
}

void Scenario::onMouseWheel(int delta)
{
   if (paused_)
      return;

   float factor = 0.85f;
   
   if (delta < 0)
   {
      delta = -delta;
      factor = 1 / factor;
   }

   while (--delta > 0)
      factor *= factor;

   zoom(factor, sf::Keyboard::isKeyPressed(sf::Keyboard::Space));
}

void Scenario::onMouseButton(sf::Mouse::Button button, bool down)
{
   if (paused_)
      return;

   input_mgr_.onMouseButton(button, down);
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
   // TODO: close warning menu
   return true;
}

void Scenario::cancelInput()
{
   input_mgr_.cancelInput();
}


} // namespace carcassonne
