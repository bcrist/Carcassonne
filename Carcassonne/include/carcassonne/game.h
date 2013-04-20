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
// File: carcassonne/game.h
//
// Main game logic.

#ifndef CARCASSONNE_GAME_H_
#define CARCASSONNE_GAME_H_
#include "carcassonne/_carcassonne.h"

#include <vector>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "carcassonne/db/db.h"
#include "carcassonne/gfx/graphics_configuration.h"
#include "carcassonne/gfx/ortho_camera.h"
#include "carcassonne/asset_manager.h"
#include "carcassonne/gui/menu.h"
#include "carcassonne/scenario.h"

namespace carcassonne {

class Game
{
public:
   Game();
   int run();

   db::DB& getConfigurationDB();
   const gfx::GraphicsConfiguration& getGraphicsConfiguration() const;
   AssetManager& getAssetManager();
   Scenario* getScenario() const;
   
   void onMouseMoved(const glm::ivec2& window_coords);
   void onMouseWheel(int delta);
   void onMouseButton(sf::Mouse::Button button, bool down);

   void onKey(const sf::Event::KeyEvent& event, bool down);
   void onCharacter(const sf::Event::TextEvent& event);

   void onResized(const glm::ivec2& new_size);
   void onBlurred();
   void onClosed();

   void close();
   void reloadGraphicsConfiguration();

   void newScenario(ScenarioInit& options);
   void endScenario();

   void pushMenu(const std::string& name);
   void pushMenu(std::unique_ptr<gui::Menu>&& menu);
   void popMenu();
   void clearMenus();
   void setMenu(const std::string& name);
   void setMenu(std::unique_ptr<gui::Menu>&& menu);

private:
   void graphicsConfigChanged();
   void createWindow();
   void initOpenGL();

   void update();
   void draw();

   db::DB config_db_;
   gfx::GraphicsConfiguration gfx_cfg_;
   sf::Window window_;
   AssetManager assets_;
   
   gfx::OrthoCamera menu_camera_;
   std::vector<std::unique_ptr<gui::Menu> > menu_stack_;

   std::unique_ptr<Scenario> scenario_;




   std::vector<Player*> players_;

};

} // namespace carcassonne

#endif
