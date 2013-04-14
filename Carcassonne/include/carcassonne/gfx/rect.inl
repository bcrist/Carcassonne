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
// File: carcassonne/gfx/rect.inl
//
// Represents an axis-aligned rectangle as the location of its top-left corner,
// along with its width and height.

#ifndef CARCASSONNE_GFX_RECT_INL_
#define CARCASSONNE_GFX_RECT_INL_

#ifndef CARCASSONNE_GFX_RECT_H_
#include "carcassonne/gfx/rect.h"
#endif

namespace carcassonne {
namespace gfx {

inline Rect::Rect()
{
}

inline Rect::Rect(const glm::vec2& position, const glm::vec2& size)
   : position(position),
     size(size)
{
}

inline float Rect::top() const
{
   return position.y;
}

inline float Rect::bottom() const
{
   return position.y + size.y;
}

inline float Rect::left() const
{
   return position.x;
}

inline float Rect::right() const
{
   return position.x + size.x;
}

inline float Rect::width() const
{
   return size.x;
}

inline float Rect::height() const
{
   return size.y;
}

inline glm::vec2 Rect::center() const
{
   return position + size * 0.5f;
}

} // namespace carcassonne::gfx
} // namespace carcassonne

#endif
