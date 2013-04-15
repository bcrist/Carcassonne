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
// File: carcassonne/gfx/camera.h

#include "carcassonne/gfx/perspective_camera.h"

#include <limits>
#include <glm/gtc/matrix_transform.hpp>

namespace carcassonne {
namespace gfx {

PerspectiveCamera::PerspectiveCamera(const GraphicsConfiguration& gfx_cfg)
   : gfx_cfg_(gfx_cfg),
     position_(0,0,0),
     target_(0,0,1),
     up_(0,1,0),
     z_near_(0.1f),
     z_far_(1000.0f),
     right_(0),
     top_(0)
{
}

void PerspectiveCamera::setPosition(const glm::vec3& position)
{
   position_ = position;
   recalculateView();
}

void PerspectiveCamera::setTarget(const glm::vec3& target)
{
   target_ = target;
   recalculateView();
}

void PerspectiveCamera::setUp(const glm::vec3& up)
{
   up_ = up;
   recalculateView();
}

void PerspectiveCamera::recalculatePerspective()
{
   const float size_y = static_cast<float>(gfx_cfg_.viewport_size.y);
   const float aspect = gfx_cfg_.viewport_size.x / size_y;
   const float half_fov_rads = glm::radians(gfx_cfg_.vertical_fov * 0.5f);

   if (half_fov_rads == 0 || z_near_ <= 0 || z_far_ <= z_near_)
      return;

   glm::mat4 projection(glm::perspective(gfx_cfg_.vertical_fov, aspect, z_near_, z_far_));
   setProjection(projection);

   top_ = tan(half_fov_rads);
   right_ = top_ * aspect;
}

void PerspectiveCamera::recalculateView()
{
   if (position_ == target_ || up_ == glm::vec3())
      return;

   glm::mat4 view(glm::lookAt(position_, target_, up_));
   setView(view);
}

glm::vec3 PerspectiveCamera::windowToWorld(const glm::vec2& window_coords, float plane_y) const
{
   // note: plane_y is assumed to be in world coords

   const float bottom = -top_;
   const float left = -right_;

   // TODO: convert window coordinates to ray direction target point using z_near_, right_, top_
   //       then convert the target point from eye coordinates to world coordinates

   glm::vec2 window_normalized(window_coords.x / gfx_cfg_.viewport_size.x,
                               window_coords.y / gfx_cfg_.viewport_size.y);
   glm::vec4 target(window_normalized.x * right_ + (1 - window_normalized.x) * left,
                    window_normalized.y * bottom + (1 - window_normalized.y) * top_,
                    -1, 1);
   glm::vec4 target_world(eyeToWorld(target));

   // TODO: create ray from camera position and target point above (direction = target - position_)

   glm::vec3 direction(glm::vec3(target_world) - position_);

   // TODO: interpolate ray intersection with the plane with constant y-coordinate plane_y
   //       by computing float t = (plane_y - position_.y) / direction.y
   //       then calculate the intersection by multiplying t * direction and adding that to the
   //       camera position.

   float delta_y = plane_y - position_.y;
   float t = -1;
   if (direction.y != 0)
      t =  delta_y / direction.y;

   if (t < 0)
      return glm::vec3(std::numeric_limits<float>::signaling_NaN(),
                       std::numeric_limits<float>::signaling_NaN(),
                       std::numeric_limits<float>::signaling_NaN());

   return position_ + direction * t;
}

} // namespace carcassonne::gfx
} // namespace carcassonne
