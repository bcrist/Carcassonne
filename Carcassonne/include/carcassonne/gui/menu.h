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
// File: carcassonne/gui/menu.h

#ifndef CARCASSONNE_GUI_MENU_H_
#define CARCASSONNE_GUI_MENU_H_
#include "carcassonne/_carcassonne.h"

#include <SFML/Window.hpp>

#include "carcassonne/gui/input_manager.h"
#include "carcassonne/gui/button.h"

namespace carcassonne {

class Game;
class AssetManager;

namespace gui {

class Menu
{
public:
   static std::unique_ptr<Menu> load(Game& game, const std::string& name);

   Menu(Game& game);
   virtual ~Menu();

   virtual std::unique_ptr<Menu> clone() const;

   virtual void onHover(const glm::vec3& coords);
   virtual void onLeftDown(const glm::vec3& coords);
   virtual void onLeftDrag(const glm::vec3& coords, const glm::vec3& down_coords);
   virtual void onLeftUp(const glm::vec3& coords, const glm::vec3& down_coords);
   virtual void onLeftCancel(const glm::vec3& coords, const glm::vec3& down_coords);

   virtual void onMouseMoved(const glm::vec3& world_coords);
   virtual void onMouseWheel(int delta);
   virtual void onMouseButton(sf::Mouse::Button button, bool down);

   virtual void onKey(const sf::Event::KeyEvent& event, bool down);
   virtual void onCharacter(const sf::Event::TextEvent& event);

   virtual void onResized();
   virtual void onBlurred();
   virtual bool onClosed();

   virtual void update();
   virtual void draw();

   virtual void cancelInput();

protected:
   Menu(const Menu& other);

   Game& game_;

   InputManager<glm::vec3> input_mgr_;

   std::vector<Button> buttons_;
};

} // namespace carcassonne::gui
} // namespace carcassonne

#endif
