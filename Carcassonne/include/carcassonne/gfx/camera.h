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
// File: carcassonne/gfx/camera.h
//
// Represents a camera and allows conversion between various coordinate spaces
//  - World space - The global space where absolute positions are relative to.
//  - Eye space - The global space transformed by the view matrix.
//  - Clip space - The eye space transformed by the projection matrix.
//  - Device space - Clip space after applying the perspective divide.

#ifndef CARCASSONNE_GFX_CAMERA_H_
#define CARCASSONNE_GFX_CAMERA_H_
#include "carcassonne/_carcassonne.h"

namespace carcassonne {
namespace gfx {

class Camera
{
public:
   Camera();

   void setProjection(const glm::mat4& projection);
   void setView(const glm::mat4& view);
   void setViewProjection(const glm::mat4& view, const glm::mat4& projection);

   const glm::mat4& getProjection() const;
   const glm::mat4& getView() const;
   const glm::mat4& getViewProjection() const;
   const glm::mat4& getInverseProjection() const;
   const glm::mat4& getInverseView() const;
   const glm::mat4& getInverseViewProjection() const;

   void use() const;

   glm::vec4 worldToEye(const glm::vec4& world_coords) const;
   glm::vec4 worldToClip(const glm::vec4& world_coords) const;
   glm::vec4 worldToDevice(const glm::vec4& world_coords) const;
   glm::vec4 eyeToClip(const glm::vec4& eye_coords) const;
   glm::vec4 eyeToDevice(const glm::vec4& eye_coords) const;
   glm::vec4 clipToDevice(const glm::vec4& clip_coords) const;

   glm::vec4 clipToEye(const glm::vec4& clip_coords) const;
   glm::vec4 clipToWorld(const glm::vec4& clip_coords) const;
   glm::vec4 eyeToWorld(const glm::vec4& eye_coords) const;   

private:
   glm::mat4 projection_;
   glm::mat4 view_;
   glm::mat4 view_projection_;

   // lazy initialization for inverse matrices
   mutable bool inv_projection_valid_;
   mutable bool inv_view_valid_;
   mutable bool inv_view_projection_valid_;
   mutable glm::mat4 inv_projection_;
   mutable glm::mat4 inv_view_;
   mutable glm::mat4 inv_view_projection_;

   Camera(const Camera&);
   void operator=(const Camera&);
};

} // namespace carcassonne::gfx
} // namespace carcassonne

#include "carcassonne/gfx/camera.inl"

#endif
