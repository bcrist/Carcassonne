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
   : state_(STATE_NORMAL),
     text_scale_(1.0f),
     text_y_offset_(0.0f),
     font_(nullptr)
{
}

Button::Button(const Button& other)
   : state_(other.state_),
     drawing_plane_(other.drawing_plane_),
     event_plane_(other.event_plane_),
     text_(other.text_),
     text_scale_(other.text_scale_),
     text_y_offset_(other.text_y_offset_),
     font_(other.font_),
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
   text_scale_ = other.text_scale_;
   text_y_offset_ = other.text_y_offset_;
   font_ = other.font_;
   action_ = other.action_;

   for (int i = 0; i < 3; ++i)
   {
      background_[i] = other.background_[i];
      text_color_[i] = other.text_color_[i];
   }

   return *this;
}

void Button::setDrawingPlane(const gfx::Rect& rect)
{
   drawing_plane_ = rect;
}

void Button::setEventPlane(const gfx::Rect& rect)
{
   event_plane_ = rect;
}

void Button::setSprite(State state, const gfx::Sprite& sprite)
{
   background_[static_cast<unsigned int>(state)] = sprite;
}

void Button::setTextColor(State state, const glm::vec4& color)
{
   text_color_[static_cast<unsigned int>(state)] = color;
}

void Button::setTextScale(float scale)
{
   text_scale_ = scale;
}

void Button::setTextYOffset(float offset)
{
   text_y_offset_ = offset;
}

void Button::setText(const std::string& content)
{
   text_ = content;
}

void Button::setFont(gfx::TextureFont* font)
{
   font_ = font;
}

void Button::setAction(const std::function<void()>& action)
{
   action_ = action;
}

void Button::onHover(const glm::vec3& coords)
{
   if (checkEventPlane(coords))
      state_ = STATE_HOVERED;
   else
      state_ = STATE_NORMAL;
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
      if (checkEventPlane(down_coords) && action_)
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
   unsigned int state = static_cast<unsigned int>(state_);

   glPushMatrix();
   glTranslatef(drawing_plane_.left(), drawing_plane_.top(), 0);
   //glScalef(drawing_plane_.width(), drawing_plane_.height(), 0);
   gfx::Texture::disableAny();
   glDisable(GL_CULL_FACE);
   glDisable(GL_DEPTH_TEST);
   //glColor3f(1,0,0);
   //glRectf(0, 0, drawing_plane_.width(), drawing_plane_.height());
   //background_[state].draw();

   glTranslatef(0, text_y_offset_, 0);
   glScalef(text_scale_, text_scale_, 1);
   glColor4fv(glm::value_ptr(text_color_[state]));

   if (font_)
      font_->print(text_, GL_MODULATE);

   glPopMatrix();
}

bool Button::checkEventPlane(const glm::vec3& coords) const
{
   return coords.x >= event_plane_.left() && coords.x < event_plane_.right() &&
          coords.y >= event_plane_.top() && coords.y < event_plane_.bottom();
}

} // namespace carcassonne::gui
} // namespace carcassonne
