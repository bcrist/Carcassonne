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
// File: carcassonne/gfx/mesh.h
//
// A mesh is a series of primitives (vertices, normals, texture coordinates) and
// a texture to apply to them.

#ifndef CARCASSONNE_GFX_MESH_H_
#define CARCASSONNE_GFX_MESH_H_
#include "carcassonne/_carcassonne.h"

#include <vector>

#include "carcassonne/gfx/texture.h"

namespace carcassonne {

class AssetManager;

namespace gfx {

class Mesh
{
public:
   Mesh(AssetManager& asset_mgr, const std::string& name);
   ~Mesh();

   void init();

   const std::string& getName() const;

   // Uses texture specified by this mesh
   // make sure depth buffer writing and GL_DEPTH_TEST are enabled before drawing!
   void draw() const;
   void draw(GLenum texture_mode) const;
   void draw(GLenum texture_mode, const glm::vec4& texture_env_color) const;

   // Uses whatever texture settings are currently set
   // make sure depth buffer writing and GL_DEPTH_TEST are enabled before drawing!
   void drawBase() const;
   
private:
   std::string name_;

   mutable GLuint display_list_id_;

   GLenum primitive_type_;

   std::vector<glm::ivec3> indices_;
   std::vector<glm::vec3> vertices_;
   std::vector<glm::vec3> normals_;
   std::vector<glm::vec3> texture_coords_;

   const Texture* texture_;

   Mesh(const Mesh&);
   void operator=(const Mesh&);
};

} // namespace carcassonne::gfx
} // namespace carcassonne

#endif
