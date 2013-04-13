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
//         Josh Douglas
// File: carcassonne/perspective_camera.h

#ifndef CARCASSONNE_PERSPECTIVE_CAMERA_H_
#define CARCASSONNE_PERSPECTIVE_CAMERA_H_
#include "carcassonne/_carcassonne.h"

#include "carcassonne/camera.h"
#include "carcassonne/graphics_configuration.h"

namespace carcassonne {

class PerspectiveCamera : public Camera
{
public:
   PerspectiveCamera(const GraphicsConfiguration& gfx_cfg);

   void setPosition(const glm::vec3& position);
   void setTarget(const glm::vec3& target);
   void setUp(const glm::vec3& up);

   void recalculatePerspective();
   void recalculateView();

   glm::vec3 windowToWorld(const glm::vec2& window_coords, float plane_y) const;

private:
   const GraphicsConfiguration& gfx_cfg_;

   glm::vec3 position_;
   glm::vec3 target_;
   glm::vec3 up_;

   float z_near_;  // the z-coordinate of the near clipping plane
   float z_far_;   // the z-coordinate of the far clipping plane

   float top_;     // the eye-space y-coord where the top clipping plane intersects z = -1
   float right_;   // the eye-space x-coord where the right clipping plane intersects z = -1

   PerspectiveCamera(const PerspectiveCamera&);
   void operator=(const PerspectiveCamera&);
};

} // namespace carcassonne

#endif
