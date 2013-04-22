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
// File: carcassonne/gfx/ortho_camera.h

#include "carcassonne/gfx/ortho_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace carcassonne {
namespace gfx {

   // TODO: add rect specifying minimum client area
OrthoCamera::OrthoCamera(const GraphicsConfiguration& gfx_cfg)
   : gfx_cfg_(gfx_cfg),
     client_(0,0, 1, 1)
{
}

void OrthoCamera::setClient(const Rect& rect)
{
   client_ = rect;
   recalculate();
}

void OrthoCamera::recalculate()
{
   float window_aspect = float(gfx_cfg_.viewport_size.x) / float(gfx_cfg_.viewport_size.y);
   float client_aspect = client_.width() / client_.height();

   float left, right, bottom, top;
   if (window_aspect > client_aspect)
   {
      top = client_.top();
      bottom = client_.bottom();

      float width = window_aspect * client_.height();
      left = client_.left() - 0.5f * (width - client_.width());
      right = left + width;
   }
   else
   {
      left = client_.left();
      right = client_.right();

      float height = client_.width() / window_aspect;
      top = client_.top() - 0.5f * (height - client_.height());
      bottom = top + height;
   }

   glm::mat4 projection(glm::ortho(left, right, bottom, top, -1.f, 1.f));
   Camera::setViewProjection(glm::mat4(), projection);
}

// calculate the world position at the window coordinates provided
glm::vec3 OrthoCamera::windowToWorld(const glm::vec2& window_coords) const
{
   return glm::vec3(window_coords, 0);
}

} // namespace carcassonne::gfx
} // namespace carcassonne
