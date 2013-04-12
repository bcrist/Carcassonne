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

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "carcassonne/db/db.h"
#include "carcassonne/graphics_configuration.h"
#include "carcassonne/assets/asset_manager.h"


namespace carcassonne {

class Game
{
public:
   Game();
   int run();
   int close();

   void createWindow();
   void initOpenGL();
   

   void resize(const glm::ivec2& new_size);

   void simulate(sf::Time delta);
   void draw();

   bool isSimulationRunning() const;
   void setSimulationRunning(bool running);

private:
   db::DB config_db_;

   GraphicsConfiguration gfx_cfg_;
   sf::Window window_;
  
   assets::AssetManager assets_;

   bool simulation_running_;
   sf::Time min_simulate_interval_;
   sf::Clock clock_;
};

} // namespace carcassonne

#endif
