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
// File: carcassonne/gui/button.cc

#include "carcassonne/gui/button.h"

namespace carcassonne {
namespace gui {

Button::Button()
   : state_(STATE_NORMAL)
{
}

Button::Button(const Button& other)
   : state_(other.state_),
     drawing_plane_(other.drawing_plane_),
     event_plane_(other.event_plane_),
     text_(other.text_),
     action_(other.action_)
{
   for (int i = 0; i < 3; ++i)
   {
      background_[i] = other.background_[i];
      text_color_[i] = other.text_color_[i];
   }
}

Button& Button::operator=(const Button& other)
{
   state_ = other.state_;
   drawing_plane_ = other.drawing_plane_;
   event_plane_ = other.event_plane_;
   text_ = other.text_;
   action_ = other.action_;

   for (int i = 0; i < 3; ++i)
   {
      background_[i] = other.background_[i];
      text_color_[i] = other.text_color_[i];
   }

   return *this;
}

void Button::setText(const std::string& content)
{
   text_ = content;
}

void Button::setAction(const std::function<void()>& action)
{
   action_ = action;
}

void Button::onHover(const glm::vec3& coords)
{
   if (checkEventPlane(coords))
      state_ = STATE_HOVERED;
}

void Button::onLeftDown(const glm::vec3& coords)
{
   if (checkEventPlane(coords))
      state_ = STATE_ACTIVE;
}

void Button::onLeftDrag(const glm::vec3& coords, const glm::vec3& down_coords)
{
   if (checkEventPlane(coords) && checkEventPlane(down_coords))
      state_ = STATE_ACTIVE;
   else
      state_ = STATE_NORMAL;
}

void Button::onLeftUp(const glm::vec3& coords, const glm::vec3& down_coords)
{
   if (checkEventPlane(coords))
   {
      state_ = STATE_HOVERED;
      if (checkEventPlane(down_coords))
         action_();
   }
}

void Button::onLeftCancel(const glm::vec3& coords, const glm::vec3& down_coords)
{
   if (checkEventPlane(coords))
      state_ = STATE_HOVERED;
}

/*void Button::update()
{
}*/

void Button::draw()
{
   glPushMatrix();
   glTranslatef(drawing_plane_.left(), drawing_plane_.top(), 0);
   glScalef(drawing_plane_.width(), drawing_plane_.height(), 0);

   background_[static_cast<unsigned int>(state_)].draw();
   glPopMatrix();
}

} // namespace carcassonne::gui
} // namespace carcassonne

#endif
