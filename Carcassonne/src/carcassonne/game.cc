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

#include "carcassonne/scheduling/delay.h"
#include "carcassonne/scheduling/sequence.h"
#include "carcassonne/scheduling/interpolator.h"
#include "carcassonne/scheduling/easing/easing.h"

namespace carcassonne {

Game::Game()
   : config_db_("carcassonne.ccconfig"),
     gfx_cfg_(gfx::GraphicsConfiguration::load(config_db_)),
     assets_("carcassonne.ccassets"),
     menu_camera_(gfx_cfg_)
{
}

int Game::run()
{
   graphicsConfigChanged();

   while (window_.isOpen())
   {
      sf::Event event;
      while (window_.pollEvent(event))
      {
         if (event.type == sf::Event::Resized)
            onResized(glm::ivec2(event.size.width, event.size.height));

         else if (event.type == sf::Event::MouseMoved)
            onMouseMoved(glm::ivec2(event.mouseMove.x, event.mouseMove.y));

         else if (event.type == sf::Event::MouseWheelMoved)
            onMouseWheel(event.mouseWheel.delta);

         else if (event.type == sf::Event::MouseButtonPressed)
            onMouseButton(event.mouseButton.button, true);

         else if (event.type == sf::Event::MouseButtonReleased)
            onMouseButton(event.mouseButton.button, false);

         else if (event.type == sf::Event::LostFocus)
            onBlurred();

         else if (event.type == sf::Event::KeyPressed)
         {
            onKey(event.key, true);

            if (event.key.code == sf::Keyboard::Escape)
               onClosed();
         }

         else if (event.type == sf::Event::KeyReleased)
            onKey(event.key, false);

         else if (event.type == sf::Event::TextEntered)
            onCharacter(event.text);

         else if (event.type == sf::Event::Closed)
            onClosed();
      }
  
      update();
      draw();
      window_.display();
   }

   return 0;
}

bool Game::close()
{
   if (!menu_stack_.empty() && !menu_stack_.back()->onClose())
      return false;

   if (scenario_ && !scenario_->onClose())
      return false;
   
   if (gfx_cfg_.save_window_location)
   {
      sf::Vector2i pos(window_.getPosition());
      gfx_cfg_.window_position.x = pos.x;
      gfx_cfg_.window_position.y = pos.y;

      gfx_cfg_.saveWindowLocation(config_db_);
   }

   window_.close();

   return true;
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

   const glm::vec4 material_specular(1,1,1,1);
   float material_shininess = 25.0f;

   const glm::vec4 light_ambient(0.6f, 0.6f, 0.6f, 1);
   const glm::vec4 light_diffuse(0.4f, 0.4f, 0.4f, 1);
   const glm::vec4 light_specular(0.2f, 0.2f, 0.2f, 1);

   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(material_specular));
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess);

   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glm::value_ptr(light_ambient));

   glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(light_diffuse));
   glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(light_specular));
   

   glEnable(GL_DEPTH_TEST);
   glEnable(GL_LIGHT0);
   glEnable(GL_COLOR_MATERIAL);

   resize(gfx_cfg_.viewport_size);
}

void Game::onResized(const glm::ivec2& new_size)
{
   gfx_cfg_.viewport_size = new_size;
   glViewport(0, 0, new_size.x, new_size.y);

   menu_camera_.recalculate();

   if (scenario_)
      scenario_->onResized();

   if (!menu_stack_.empty())
      menu_stack_.back()->onResized();
}

void Game::onBlurred()
{
   if (!menu_stack_.empty())
      menu_stack_.back()->onBlurred();

   if (scenario_)
      scenario_->onBlurred();
}

void Game::onClosed()
{
   if (!menu_stack_.empty())
      menu_stack_.back()->onClose();

   if (scenario_)
      scenario_->onClose();
}

void Game::onMouseMoved(const glm::ivec2& window_coords)
{
   if (scenario_)
      scenario_->onMouseMoved(window_coords);

   if (!menu_stack_.empty())
   {
      glm::vec3 world_coords(menu_camera_.windowToWorld(window_coords));
      menu_stack_.back()->onMouseMoved(world_coords);
   }
}

void Game::update()
{
   if (scenario_)
      scenario_->update();

   if (!menu_stack_.empty())
      menu_stack_.back()->update();
}

void Game::draw()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   if (scenario_)
      scenario_->draw();

   if (!menu_stack_.empty())
   {
      glDisable(GL_LIGHTING);
      menu_camera_.use();
      menu_stack_.back()->draw();
   }
}

void Game::pushMenu(std::unique_ptr<gui::Menu>&& menu)
{
   assert(menu);  // menu.get() != nullptr

   if (!menu_stack_.empty())
      menu_stack_.back()->cancelInput();

   menu_stack_.push_back(std::move(menu));

   if (leve != NULL)
      level->pause();
}

void Game::popMenu()
{
	if (!menuStack.empty())
	{
		menuStack.back()->setVisibility(false);
		menuStack.pop_back();
		
		if (menuStack.empty())
		{
			if (level == NULL)
				pushMenu(mainMenu);
			else
				level->resume();
		}
		else
			menuStack.back()->setVisibility(true);
	}
}

void Game::clearMenu()
{
	for(mstack_iter_t it = menuStack.begin(); it != menuStack.end(); ++it)
		(*it)->setVisibility(false);

	menuStack.clear();

	if (level == NULL)
		pushMenu(mainMenu);
	else
		level->resume();
}



} // namespace carcassonne
