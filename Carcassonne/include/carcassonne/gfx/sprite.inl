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
// File: carcassonne/gfx/sprite.inl
//
// A sprite is a combination of a texture and a set of coordinates specifying
// which part of the texture to draw.

#ifndef CARCASSONNE_GFX_SPRITE_INL_
#define CARCASSONNE_GFX_SPRITE_INL_

#ifndef CARCASSONNE_GFX_SPRITE_H_
#include "carcassonne/gfx/sprite.h"
#endif

#include <glm/gtc/type_ptr.hpp>

namespace carcassonne {
namespace gfx {

inline Sprite::Sprite()
   : texture(nullptr)
{
}

inline Sprite::Sprite(const Texture& texture, const Rect& texture_coords)
   : texture(&texture),
     texture_coords(texture_coords)
{
}

// draw the sprite in the XY plane from (0,0,0) to (1,1,0)
inline void Sprite::draw() const
{
   if (texture)
      texture->enable(GL_MODULATE);
   else
      Texture::disableAny();

   glBegin(GL_QUADS);
      glTexCoord2f(texture_coords.left(),  texture_coords.top());    glVertex2f(0, 1);
      glTexCoord2f(texture_coords.left(),  texture_coords.bottom()); glVertex2f(0, 0);
      glTexCoord2f(texture_coords.right(), texture_coords.bottom()); glVertex2f(1, 0);
      glTexCoord2f(texture_coords.right(), texture_coords.top());    glVertex2f(1, 1);
   glEnd();
}

inline void Sprite::draw(GLenum mode) const
{
   if (texture)
      texture->enable(mode);
   else
      Texture::disableAny();

   glBegin(GL_QUADS);
      glTexCoord2f(texture_coords.left(),  texture_coords.top());    glVertex2f(0, 1);
      glTexCoord2f(texture_coords.left(),  texture_coords.bottom()); glVertex2f(0, 0);
      glTexCoord2f(texture_coords.right(), texture_coords.bottom()); glVertex2f(1, 0);
      glTexCoord2f(texture_coords.right(), texture_coords.top());    glVertex2f(1, 1);
   glEnd();
}

inline void Sprite::draw(GLenum mode, const glm::vec4& color) const
{
   if (texture)
      texture->enable(mode, color);
   else
      Texture::disableAny();

   glBegin(GL_QUADS);
      glTexCoord2f(texture_coords.left(),  texture_coords.top());    glVertex2f(0, 1);
      glTexCoord2f(texture_coords.left(),  texture_coords.bottom()); glVertex2f(0, 0);
      glTexCoord2f(texture_coords.right(), texture_coords.bottom()); glVertex2f(1, 0);
      glTexCoord2f(texture_coords.right(), texture_coords.top());    glVertex2f(1, 1);
   glEnd();
}

} // namespace carcassonne::gfx
} // namespace carcassonne

#endif
