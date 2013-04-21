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
// File: carcassonne/gui/input_manager.cc

#include "carcassonne/gui/input_manager.h"

#include <functional>
#include <set>
#include <SFML/Window.hpp>

namespace carcassonne {
namespace gui {

InputManager::InputManager()
{
   btn_down_[0] = false;
   btn_down_[1] = false;
   btn_down_[2] = false;
}

void InputManager::onMouseMoved(const glm::ivec2& window_coords)
{
   last_delta_ = window_coords - current_pos_;
   current_pos_ = window_coords;

   for (int i = 0; i < 3; ++i)
   {
      if (btn_down_[i] && drag_handler_[i])
         drag_handler_[i](btn_down_pos_[i]);
   }

   if (!(btn_down_[0] || btn_down_[1] || btn_down_[2]) && hover_handler_)
      hover_handler_();
}


void InputManager::onMouseButton(sf::Mouse::Button button, bool down)
{
   int btn = 0;
   if (button == sf::Mouse::Right)
      btn = 1;
   else if (button == sf::Mouse::Middle)
      btn = 2;

   if (down)
   {
      if (btn_down_[btn] && cancel_handler_[btn])
         cancel_handler_[btn](btn_down_pos_[btn]);

      btn_down_[btn] = true;
      btn_down_pos_[btn] = current_pos_;
      if (down_handler_[btn])
         down_handler_[btn]();  
   }
   else if (btn_down_[btn])
   {
      btn_down_[btn] = false;
      if (up_handler_[btn])
         up_handler_[btn](btn_down_pos_[btn]);
   }
}

void InputManager::onKey(const sf::Event::KeyEvent& event, bool down)
{
   for (int i = 0; i < 3; ++i)
   {
      if (btn_down_[i])
      {
         if (cancel_handler_[i])
            cancel_handler_[i](btn_down_pos_[i]);
         btn_down_pos_[i] = current_pos_;
         if (down_handler_[i])
            down_handler_[i]();
      }
   }
}

   
void InputManager::cancelInput()
{
   for (int i = 0; i < 3; ++i)
   {
      if (btn_down_[i] && cancel_handler_[i])
         cancel_handler_[i](btn_down_pos_[i]);
      btn_down_[i] = false;
   }
}

const glm::ivec2& InputManager::getMousePosition() const
{
   return current_pos_;
}

const glm::ivec2& InputManager::getMouseDelta() const
{
   return last_delta_;
}

void InputManager::setMouseDownHandler(sf::Mouse::Button button, const std::function<void()>& func)
{
   int btn = 0;
   if (button == sf::Mouse::Right)
      btn = 1;
   else if (button == sf::Mouse::Middle)
      btn = 2;

   down_handler_[btn] = func;
}

void InputManager::setMouseDragHandler(sf::Mouse::Button button, const std::function<void(const glm::ivec2&)>& func)
{
   int btn = 0;
   if (button == sf::Mouse::Right)
      btn = 1;
   else if (button == sf::Mouse::Middle)
      btn = 2;

   drag_handler_[btn] = func;
}

void InputManager::setMouseUpHandler(sf::Mouse::Button button, const std::function<void(const glm::ivec2&)>& func)
{
   int btn = 0;
   if (button == sf::Mouse::Right)
      btn = 1;
   else if (button == sf::Mouse::Middle)
      btn = 2;

   up_handler_[btn] = func;
}

void InputManager::setMouseCancelHandler(sf::Mouse::Button button, const std::function<void(const glm::ivec2&)>& func)
{
   int btn = 0;
   if (button == sf::Mouse::Right)
      btn = 1;
   else if (button == sf::Mouse::Middle)
      btn = 2;

   cancel_handler_[btn] = func;
}

void InputManager::setMouseHoverHandler(const std::function<void()>& func)
{
   hover_handler_ = func;
}

} // namespace carcassonne::gui
} // namespace carcassonne
