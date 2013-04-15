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
// File: carcassonne/gfx/sprite.h
//
// A sprite is a combination of a texture and a set of coordinates specifying
// which part of the texture to draw.

#ifndef CARCASSONNE_GFX_SPRITE_H_
#define CARCASSONNE_GFX_SPRITE_H_
#include "carcassonne/_carcassonne.h"

#include "carcassonne/db/db.h"
#include "carcassonne/gfx/texture.h"
#include "carcassonne/gfx/rect.h"

namespace carcassonne {

class AssetManager;

namespace gfx {

struct Sprite
{
   Sprite();
   Sprite(const Texture& texture, const Rect& texture_coords);
   Sprite(AssetManager& asset_mgr, const std::string& name);
   
   // draw the sprite in the XY plane from (0,0,0) to (1,1,0)
   void draw() const;
   void draw(GLenum texture_mode) const;
   void draw(GLenum texture_mode, const glm::vec4& texture_env_color) const;

   const Texture* texture;
   Rect texture_coords;
};

} // namespace carcassonne::gfx
} // namespace carcassonne

#include "carcassonne/gfx/sprite.inl"

#endif
