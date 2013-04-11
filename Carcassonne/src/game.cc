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
// File: game.cc
//
// Main game logic.

#include "game.h"

#include <SFML/OpenGL.hpp>

#include "db/transaction.h"
#include "db/stmt.h"

namespace carcassonne {

Game::Game()
   : config_db_("carcassonne.config"),
     simulation_running_(true),
     min_simulate_interval_(sf::milliseconds(5))
{
}

int Game::run()
{
   wnd_.create(sf::VideoMode(640, 480), "Carcassonne!");
   
   initOpenGL();
   clock_.restart();

   while (wnd_.isOpen())
   {
      sf::Event event;
      while (wnd_.pollEvent(event))
      {
         if (event.type == sf::Event::Resized)
         {
            sf::Event::SizeEvent& se = event.size;
            resize(glm::ivec2(se.width, se.height));
         }

         if (event.type == sf::Event::Closed)
            wnd_.close();
      }
  
      // Simulate  Draw a new frame
      if (simulation_running_ && clock_.getElapsedTime() >= min_simulate_interval_)
      {
         sf::Time delta = clock_.restart();
         simulate(delta);
      }

      draw();
      wnd_.display();
   }

   return 0;
}

void Game::initOpenGL()
{
   glClearColor(0,0,0,0);

   glMatrixMode(GL_PROJECTION);

   glMatrixMode(GL_MODELVIEW);
}

void Game::resize(const glm::ivec2& new_size)
{
   viewport_size_ = new_size;
}

void Game::simulate(sf::Time delta)
{
}

void Game::draw()
{
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
