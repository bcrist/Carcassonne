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
// File: carcassonne/camera.h

#ifndef CARCASSONNE_CAMERA_H_
#define CARCASSONNE_CAMERA_H_
#include "carcassonne/_carcassonne.h"

namespace carcassonne {

class Camera
{
public:
   Camera();

   void setProjection(const glm::mat4& projection);
   void setView(const glm::mat4& view);

   const glm::mat4& getProjection() const;
   const glm::mat4& getView() const;

   void draw() const;

   glm::vec3 worldToEye(const glm::vec3& world_coords) const;
   glm::vec3 worldToClip(const glm::vec3& world_coords) const;
   glm::vec2 worldToWindow(const glm::vec3& world_coords) const;
   glm::vec3 eyeToClip(const glm::vec3& eye_coords) const;
   glm::vec2 eyeToWindow(const glm::vec3& eye_coords) const;
   glm::vec2 clipToWindow(const glm::vec3& clip_coords) const;

   glm::vec3 clipToEye(const glm::vec3& clip_coords) const;
   glm::vec3 clipToWorld(const glm::vec3& clip_coords) const;
   glm::vec3 eyeToWorld(const glm::vec3& eye_coords) const;   

private:
   glm::mat4 projection_;
   glm::mat4 view_;

   Camera(const Camera&);
   void operator=(const Camera&);
};

} // namespace carcassonne

#endif
