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
//         Josh Douglas
// File: carcassonne/game.cc
//
// Main game logic.

#include "carcassonne/game.h"

#include <limits>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/OpenGL.hpp>

#include "carcassonne/db/transaction.h"
#include "carcassonne/db/stmt.h"

namespace carcassonne {

Game::Game()
   : config_db_("carcassonne.ccconfig"),
     gfx_cfg_(gfx::GraphicsConfiguration::load(config_db_)),
     assets_("carcassonne.ccassets"),
     simulation_running_(true),
     min_simulate_interval_(sf::milliseconds(5)),
     game_camera_(gfx_cfg_),
     gui_camera_(gfx_cfg_)
{
}

int Game::run()
{
   graphicsConfigChanged();
   clock_.restart();

   game_camera_.setPosition(glm::vec3(10, 10, 10));
   game_camera_.setTarget(glm::vec3(0,0,0));

   gfx::Texture* tex(assets_.getTexture("test"));
   tex->enable(GL_REPLACE);
   
   while (window_.isOpen())
   {
      sf::Event event;
      while (window_.pollEvent(event))
      {
         if (event.type == sf::Event::Resized)
         {
            sf::Event::SizeEvent& se = event.size;
            resize(glm::ivec2(se.width, se.height));
         }

         else if (event.type == sf::Event::MouseMoved)
         {
            sf::Event::MouseMoveEvent& mme = event.mouseMove;
            mouseMove(glm::ivec2(mme.x, mme.y));
         }

         else if (event.type == sf::Event::Closed)
            return close();
      }
  
      // Simulate  Draw a new frame
      if (simulation_running_ && clock_.getElapsedTime() >= min_simulate_interval_)
      {
         sf::Time delta = clock_.restart();
         simulate(delta);
      }

      draw();
      window_.display();
   }

   return 0;
}

int Game::close()
{
   int return_value = 0;

   if (gfx_cfg_.save_window_location)
   {
      sf::Vector2i pos(window_.getPosition());
      gfx_cfg_.window_position.x = pos.x;
      gfx_cfg_.window_position.y = pos.y;

      return_value = gfx_cfg_.saveWindowLocation(config_db_);
   }

   window_.close();

   return return_value;
}

void Game::graphicsConfigChanged()
{
   createWindow();
   initOpenGL();
}

void Game::createWindow()
{
   sf::VideoMode mode(gfx_cfg_.viewport_size.x, gfx_cfg_.viewport_size.y,
                      gfx_cfg_.color_bits > 0 ? gfx_cfg_.color_bits : sf::VideoMode::getDesktopMode().bitsPerPixel);
   
   if (gfx_cfg_.window_mode == gfx::GraphicsConfiguration::WINDOW_MODE_FULLSCREEN_WINDOWED)
      mode = sf::VideoMode::getDesktopMode();

   else if (gfx_cfg_.window_mode == gfx::GraphicsConfiguration::WINDOW_MODE_FULLSCREEN_EXCLUSIVE)
   {
      const std::vector<sf::VideoMode>& modes = sf::VideoMode::getFullscreenModes();

      float best_score(0);
      const sf::VideoMode* best_mode(nullptr);
      for (auto i(modes.begin()), end(modes.end()); i != end; ++i)
      {
         const sf::VideoMode& mode(*i);

         float score(std::numeric_limits<float>::min());
         score += std::abs(static_cast<float>(static_cast<int>(mode.height) - gfx_cfg_.viewport_size.y))
                     / gfx_cfg_.viewport_size.y;
         score += std::abs(static_cast<float>(static_cast<int>(mode.width) - gfx_cfg_.viewport_size.x))
                     / gfx_cfg_.viewport_size.x;

         if (mode.bitsPerPixel != gfx_cfg_.color_bits)
            score += 0.1f + score;

         score = 1 - score;

         if (best_mode == nullptr || score > best_score)
         {
            best_mode = &mode;
            best_score = score;
         }
      }
      mode = *best_mode;
   }

   sf::Uint32 style;
   switch (gfx_cfg_.window_mode)
   {
      case gfx::GraphicsConfiguration::WINDOW_MODE_FIXED:
         style = sf::Style::Titlebar | sf::Style::Close;
         break;

      case gfx::GraphicsConfiguration::WINDOW_MODE_FULLSCREEN_WINDOWED:
         style = sf::Style::None;
         break;

      case gfx::GraphicsConfiguration::WINDOW_MODE_FULLSCREEN_EXCLUSIVE:
         style = sf::Style::Fullscreen;
         break;

      default: // WINDOW_MODE_RESIZABLE
         style = sf::Style::Default;
         break;
   }

   sf::ContextSettings settings(gfx_cfg_.depth_bits,
                                gfx_cfg_.stencil_bits,
                                gfx_cfg_.msaa_level,
                                gfx_cfg_.gl_version_major,
                                gfx_cfg_.gl_version_minor);

   window_.create(mode, "Carcassonne!", style, settings);
   window_.setVerticalSyncEnabled(gfx_cfg_.v_sync);

   if (gfx_cfg_.window_mode == gfx::GraphicsConfiguration::WINDOW_MODE_FULLSCREEN_WINDOWED)
   {
      window_.setPosition(sf::Vector2i(0, 0));
   }
   else if (gfx_cfg_.window_mode != gfx::GraphicsConfiguration::WINDOW_MODE_FULLSCREEN_EXCLUSIVE &&
            gfx_cfg_.save_window_location)
   {
      window_.setPosition(sf::Vector2i(gfx_cfg_.window_position.x, gfx_cfg_.window_position.y));
   }

   // Overwrite GraphicsConfiguration values with data from actual values
   settings = window_.getSettings();
   gfx_cfg_.depth_bits = settings.depthBits;
   gfx_cfg_.stencil_bits = settings.stencilBits;
   gfx_cfg_.msaa_level = settings.antialiasingLevel;
   gfx_cfg_.gl_version_major = settings.majorVersion;
   gfx_cfg_.gl_version_minor = settings.minorVersion;

   sf::Vector2u window_size(window_.getSize());
   gfx_cfg_.viewport_size = glm::ivec2(window_size.x, window_size.y);
   gfx_cfg_.color_bits = mode.bitsPerPixel;
}

void Game::initOpenGL()
{
   glClearColor(0,0,0,0);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   resize(gfx_cfg_.viewport_size);
}

void Game::resize(const glm::ivec2& new_size)
{
   gfx_cfg_.viewport_size = new_size;
   glViewport(0, 0, new_size.x, new_size.y);

   game_camera_.recalculatePerspective();
   gui_camera_.recalculate();
}

void Game::mouseMove(const glm::ivec2& window_coords)
{
   glm::vec3 new_hover_position = game_camera_.windowToWorld(glm::vec2(window_coords), hover_position_.y);
   hover_position_.x = new_hover_position.x;
   hover_position_.z = new_hover_position.z;
}

void Game::simulate(sf::Time delta)
{
}

void Game::draw()
{
   glClear(GL_COLOR_BUFFER_BIT);
   game_camera_.use();

   glPushMatrix();
   glTranslatef(hover_position_.x, hover_position_.y, hover_position_.z);

   glColor3f(0,1,1);

   glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex3f(-1, 0, -1);
      glTexCoord2f(0, 1); glVertex3f(-1, 0,  1);
      glTexCoord2f(1, 1); glVertex3f( 1, 0,  1);
      glTexCoord2f(1, 0); glVertex3f( 1, 0, -1);
   glEnd();

   glPopMatrix();
}

bool Game::isSimulationRunning() const
{
   return simulation_running_;
}

void Game::setSimulationRunning(bool running)
{
   if (simulation_running_ != running)
   {
      simulation_running_ = running;

      if (running)
         clock_.restart();
   }
}

} // namespace carcassonne
