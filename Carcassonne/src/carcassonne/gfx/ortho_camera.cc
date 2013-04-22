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
   : gfx_cfg_(gfx_cfg)
{
}

void OrthoCamera::recalculate()
{
   glm::mat4 projection(glm::ortho(0.f, float(gfx_cfg_.viewport_size.x), float(gfx_cfg_.viewport_size.y), float(0), -1.f, 1.f));
   Camera::setViewProjection(glm::mat4(), projection);
   // TODO: calculate projection to fit client area inside viewport

   // TODO: use identity view matrix
}

// calculate the world position at the window coordinates provided
glm::vec3 OrthoCamera::windowToWorld(const glm::vec2& window_coords) const
{
   return glm::vec3(window_coords, 0);
}

} // namespace carcassonne::gfx
} // namespace carcassonne
