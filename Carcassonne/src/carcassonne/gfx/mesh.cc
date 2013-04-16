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
   size_t n_indices, n_vertices, n_normals, n_texture_coords;
   {
      db::Stmt stmt(db, "SELECT texture, primitive_type, id, indices, vertices, normals, texture_coords "
                        "FROM cc_meshes "
                        "WHERE name = ? LIMIT 1");
      stmt.bind(1, name);
      if (!stmt.step())
         throw db::DB::error("Mesh not found!");

      texture_ = asset_mgr.getTexture(stmt.getText(0));

      switch (stmt.getInt(1))
      {
         case 0: primitive_type_ = GL_TRIANGLES; break;
         case 1: primitive_type_ = GL_TRIANGLE_STRIP; break;
         case 2: primitive_type_ = GL_TRIANGLE_FAN; break;
         case 3: primitive_type_ = GL_QUADS; break;
         case 4: primitive_type_ = GL_QUAD_STRIP; break;
         case 5: primitive_type_ = GL_POLYGON; break;
         case 6: primitive_type_ = GL_POINTS; break;
         case 7: primitive_type_ = GL_LINES; break;
         case 8: primitive_type_ = GL_LINE_STRIP; break;
         case 9: primitive_type_ = GL_LINE_LOOP; break;
         default: throw std::runtime_error("Unknown mesh type!");
      }

      id = stmt.getInt(2);
      n_indices = stmt.getInt(3);
      n_vertices = stmt.getInt(4);
      n_normals = stmt.getInt(5);
      n_texture_coords = stmt.getInt(6);
   }
   indices_.reserve(n_indices);
   vertices_.reserve(n_vertices);
   normals_.reserve(n_normals);
   texture_coords_.reserve(n_texture_coords);

   {
      db::Stmt stmt(db, "SELECT type, x, y, z "
                        "FROM cc_mesh_data "
                        "WHERE mesh_id = ? "
                        "ORDER BY mesh_id, type, index");
      stmt.bind(1, id);
      while (stmt.step())
      {
         switch (stmt.getInt(0))
         {
            case 0:
               {
                  glm::ivec3 i(stmt.getInt(1), stmt.getInt(2), stmt.getInt(3));
                  if (static_cast<size_t>(i.x) >= n_vertices)
                     throw std::runtime_error("Vertex index out of range!");
                  if (static_cast<size_t>(i.y) >= n_normals)
                     throw std::runtime_error("Normal index out of range!");
                  if (static_cast<size_t>(i.z) >= n_texture_coords)
                     throw std::runtime_error("Texture coordinate index out of range!");
                  indices_.push_back(i);
               }
               break;

            case 1: vertices_.push_back(glm::vec3(float(stmt.getDouble(1)), float(stmt.getDouble(2)), float(stmt.getDouble(3)))); break;
            case 2: normals_.push_back(glm::vec3(float(stmt.getDouble(1)), float(stmt.getDouble(2)), float(stmt.getDouble(3)))); break;
            case 3: texture_coords_.push_back(glm::vec3(float(stmt.getDouble(1)), float(stmt.getDouble(2)), float(stmt.getDouble(3)))); break;
            default:
               std::cerr << "Unrecognized mesh data type: '" << stmt.getInt(0) << "', skipping!" << std::endl;
               break;
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
