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
// A mesh is a series of triangles and associated vertices, normals, texture
// coordinates, and texture.

#include "carcassonne/gfx/mesh.h"

#include <glm/gtc/type_ptr.hpp>

#include "carcassonne/asset_manager.h"
#include "carcassonne/db/stmt.h"

namespace carcassonne {
namespace gfx {


Mesh::Mesh(AssetManager& asset_mgr, const std::string& name)
{
   db::DB& db(asset_mgr.getDB());

   int id;
   {
      db::Stmt stmt(db, "SELECT texture, primitive_type, id "
                        "FROM cc_meshes "
                        "WHERE name = ? LIMIT 1");
      stmt.bind(1, name);
      if (!stmt.step())
         throw db::DB::error("Mesh not found!");

      texture_ = asset_mgr.getTexture(stmt.getText(0));

      switch (stmt.getInt(1))
      {
         case 1: primitive_type_ = GL_TRIANGLE_STRIP; break;
         case 2: primitive_type_ = GL_TRIANGLE_FAN; break;
         case 3: primitive_type_ = GL_QUADS; break;
         case 4: primitive_type_ = GL_QUAD_STRIP; break;
         case 5: primitive_type_ = GL_POLYGON; break;
         case 6: primitive_type_ = GL_POINTS; break;
         case 7: primitive_type_ = GL_LINES; break;
         case 8: primitive_type_ = GL_LINE_STRIP; break;
         case 9: primitive_type_ = GL_LINE_LOOP; break;
         default: primitive_type_ = GL_TRIANGLES; break;
      }

      id = stmt.getInt(2);
   }

   {
      // CREATE TABLE IF NOT EXISTS cc_mesh_data (
      // mesh_id INTEGER,
      // type INTEGER,
      // index INTEGER,
      // x INTEGER,
      // y INTEGER,
      // z INTEGER,
      // PRIMARY KEY (mesh_id, type, index)
      db::Stmt stmt(db, "SELECT type, x, y, z "
                        "FROM cc_mesh_data "
                        "WHERE mesh_id = ? "
                        "ORDER BY mesh_id, type, index");
      stmt.bind(1, id);
      while (stmt.step())
      {
         switch (stmt.getInt(0))
         {
            case 0: indices_.push_back(glm::ivec3(stmt.getInt(1), stmt.getInt(2), stmt.getInt(3))); break;
            case 1: vertices_.push_back(glm::vec3(float(stmt.getDouble(1)), float(stmt.getDouble(2)), float(stmt.getDouble(3)))); break;
            case 2: normals_.push_back(glm::vec3(float(stmt.getDouble(1)), float(stmt.getDouble(2)), float(stmt.getDouble(3)))); break;
            case 3: texture_coords_.push_back(glm::vec3(float(stmt.getDouble(1)), float(stmt.getDouble(2)), float(stmt.getDouble(3)))); break;
         }
      }

      for (auto i(indices_.begin()), end(indices_.end()); i != end; ++i)
      {
         if (i->x < 0 || i->x >= vertices_.size() ||
             i->y < 0 || i->y >= normals_.size() ||
             i->z < 0 || i->z >= texture_coords_.size())
         {
            throw std::runtime_error("Index out of bounds!");
         }
      }
   }
}

void Mesh::draw() const
{
   if (texture_)
      texture_->enable(GL_MODULATE);
   else
      Texture::disableAny();

   drawCommon();
}

void Mesh::draw(GLenum texture_mode) const
{
   if (texture_)
      texture_->enable(texture_mode);
   else
      Texture::disableAny();

   drawCommon();
}

void Mesh::draw(GLenum texture_mode, const glm::vec4& texture_env_color) const
{
   if (texture_)
      texture_->enable(texture_mode, texture_env_color);
   else
      Texture::disableAny();

   drawCommon();
}

void Mesh::drawCommon() const
{
   glBegin(primitive_type_);

   for (auto i(indices_.begin()), end(indices_.end()); i != end; ++i)
   {
      glTexCoord3fv(glm::value_ptr(texture_coords_[i->z]));
      glNormal3fv(glm::value_ptr(normals_[i->y]));
      glVertex3fv(glm::value_ptr(vertices_[i->x]));
   }

   glEnd();
}

} // namespace carcassonne::gfx
} // namespace carcassonne
