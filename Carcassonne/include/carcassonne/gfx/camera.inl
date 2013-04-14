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
// File: carcassonne/gfx/camera.inl
//
// Represents a camera and allows conversion between various coordinate spaces
//  - World space - The global space where absolute positions are relative to.
//  - Eye space - The global space transformed by the view matrix.
//  - Clip space - The eye space transformed by the projection matrix.
//  - Device space - Clip space after applying the perspective divide.

#ifndef CARCASSONNE_GFX_CAMERA_INL_
#define CARCASSONNE_GFX_CAMERA_INL_

#ifndef CARCASSONNE_GFX_CAMERA_H_
#include "carcassonne/gfx/camera.h"
#endif

#include <glm/gtc/type_ptr.hpp>
#include <SFML/OpenGL.hpp>

namespace carcassonne {
namespace gfx {

inline Camera::Camera()
   : inv_projection_valid_(false),
     inv_view_valid_(false),
     inv_view_projection_valid_(false)
{
}

inline void Camera::setProjection(const glm::mat4& projection)
{
   projection_ = projection;
   view_projection_ = projection_ * view_;
   inv_projection_valid_ = false;
   inv_view_projection_valid_ = false;
}

inline void Camera::setView(const glm::mat4& view)
{
   view_ = view;
   view_projection_ = projection_ * view_;
   inv_view_valid_ = false;
   inv_view_projection_valid_ = false;
}

inline void Camera::setViewProjection(const glm::mat4& view, const glm::mat4& projection)
{
   projection_ = projection;
   view_ = view;
   view_projection_ = projection_ * view_;

   inv_projection_valid_ = false;
   inv_view_valid_ = false;
   inv_view_projection_valid_ = false;
}

inline const glm::mat4& Camera::getProjection() const
{
   return projection_;
}

inline const glm::mat4& Camera::getView() const
{
   return view_;
}

inline const glm::mat4& Camera::getViewProjection() const
{
   return view_projection_;
}

inline const glm::mat4& Camera::getInverseProjection() const
{
   if (!inv_projection_valid_)
   {
      inv_projection_ = glm::inverse(projection_);
      inv_projection_valid_ = true;
   }
   return inv_projection_;
}

inline const glm::mat4& Camera::getInverseView() const
{
   if (!inv_view_valid_)
   {
      inv_view_ = glm::inverse(view_);
      inv_view_valid_ = true;
   }
   return inv_view_;
}

inline const glm::mat4& Camera::getInverseViewProjection() const
{
   if (!inv_view_projection_valid_)
   {
      inv_view_projection_ = glm::inverse(view_projection_);
      inv_view_projection_valid_ = true;
   }
   return inv_view_projection_;
}

inline void Camera::use() const
{
   glMatrixMode(GL_PROJECTION);
   glLoadMatrixf(glm::value_ptr(projection_));

   glMatrixMode(GL_MODELVIEW);
   glLoadMatrixf(glm::value_ptr(view_));
}

inline glm::vec4 Camera::worldToEye(const glm::vec4& world_coords) const
{
   return view_ * world_coords;
}
inline glm::vec4 Camera::worldToClip(const glm::vec4& world_coords) const
{
   return view_projection_ * world_coords;
}
inline glm::vec4 Camera::worldToDevice(const glm::vec4& world_coords) const
{
   glm::vec4 temp(view_projection_ * world_coords);
   temp *= 1 / temp.w;
   return temp;
}
inline glm::vec4 Camera::eyeToClip(const glm::vec4& eye_coords) const
{
   return projection_ * eye_coords;
}
inline glm::vec4 Camera::eyeToDevice(const glm::vec4& eye_coords) const
{
   glm::vec4 temp(projection_ * eye_coords);
   temp *= 1 / temp.w;
   return temp;
}
inline glm::vec4 Camera::clipToDevice(const glm::vec4& clip_coords) const
{
   return clip_coords * (1 / clip_coords.w);
}


inline glm::vec4 Camera::clipToEye(const glm::vec4& clip_coords) const
{
   if (!inv_projection_valid_)
   {
      inv_projection_ = glm::inverse(projection_);
      inv_projection_valid_ = true;
   }
   return inv_projection_ * clip_coords;
}
inline glm::vec4 Camera::clipToWorld(const glm::vec4& clip_coords) const
{
   if (!inv_view_projection_valid_)
   {
      inv_view_projection_ = glm::inverse(view_projection_);
      inv_view_projection_valid_ = true;
   }
   return inv_view_projection_ * clip_coords;
}
inline glm::vec4 Camera::eyeToWorld(const glm::vec4& eye_coords) const
{
   if (!inv_view_valid_)
   {
      inv_view_ = glm::inverse(view_);
      inv_view_valid_ = true;
   }
   return inv_view_ * eye_coords;
}

} // namespace carcassonne::gfx
} // namespace carcassonne

#endif
