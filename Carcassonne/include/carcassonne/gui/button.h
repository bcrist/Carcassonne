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
// File: carcassonne/gui/button.h

#ifndef CARCASSONNE_GUI_BUTTON_H_
#define CARCASSONNE_GUI_BUTTON_H_
#include "carcassonne/_carcassonne.h"

#include <functional>

#include "carcassonne/gfx/sprite.h"
#include "carcassonne/gfx/texture_font.h"

namespace carcassonne {
namespace gui {

class Button
{
public:
   enum State {
      STATE_NORMAL = 0,
      STATE_HOVERED = 1,
      STATE_ACTIVE = 2
   };

   Button();
   Button(const Button& other);
   Button& operator=(const Button& other);

   void setDrawingPlane(const gfx::Rect& rect);
   void setEventPlane(const gfx::Rect& rect);

   void setSprite(State state, const gfx::Sprite& sprite);

   void setTextColor(State state, const glm::vec4& color);
   void setTextScale(float scale);
   void setTextYOffset(float offset);
   void setText(const std::string& content);
   void setFont(gfx::TextureFont* font);

   void setAction(const std::function<void()>& action);

   void onHover(const glm::vec3& coords);
   void onLeftDown(const glm::vec3& coords);
   void onLeftDrag(const glm::vec3& coords, const glm::vec3& down_coords);
   void onLeftUp(const glm::vec3& coords, const glm::vec3& down_coords);
   void onLeftCancel(const glm::vec3& coords, const glm::vec3& down_coords);

   //void update();
   void draw();

private:
   bool checkEventPlane(const glm::vec3& coords) const;

   State state_;

   gfx::Rect drawing_plane_;
   gfx::Rect event_plane_;

   gfx::Sprite background_[3];
   glm::vec4 text_color_[3];
   std::string text_;
   gfx::TextureFont* font_;

   float text_scale_;
   float text_y_offset_;

   std::function<void()> action_;
};

} // namespace carcassonne::gui
} // namespace carcassonne

#endif
