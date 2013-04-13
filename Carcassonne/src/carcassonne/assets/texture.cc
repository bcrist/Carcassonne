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
// File: carcassonne/assets/texture.cc

#include "carcassonne/assets/texture.h"

#include <cassert>
#include <algorithm>

#include "stb_image.h"

#include "carcassonne/db/stmt.h"

namespace carcassonne {
namespace assets {

Texture::State Texture::state_(UNKNOWN);
GLuint Texture::bound_id_(0);
GLenum Texture::mode_(GL_MODULATE);
glm::vec4 Texture::color_(0,0,0,0);

Texture::Texture(const GLubyte* data, const glm::ivec2& size)
   : size_(size)
{
   assert(data != nullptr);
   assert(size.x > 0);
   assert(size.y > 0);

   upload(data);
}

Texture::Texture(db::DB& db, const std::string& name)
{
   db::Stmt stmt(db, "SELECT format, width, height, data "
                     "FROM cc_textures "
                     "WHERE name = ? LIMIT 1");
   stmt.bind(1, name);
   if (!stmt.step())
      throw db::DB::error("Texture not found!");

   std::string format = stmt.getText(0);
   std::transform(format.begin(), format.end(), format.begin(), tolower);
   size_.x = stmt.getInt(1);
   size_.y = stmt.getInt(2);

   const void* data;
   void* stbi_data = nullptr;

   if (format == "raw")
   {
      int length = stmt.getBlob(3, data);

      if (size_.x * size_.y * 4 > length)
         throw std::runtime_error("Raw texture data corrupted or incomplete!");
   }
   else
   {
      // load texture data using STB Image library
      int length = stmt.getBlob(3, data);
      int comps;

      stbi_data = stbi_load_from_memory(static_cast<const stbi_uc*>(data),
                                        length, &size_.x, &size_.y, &comps, 4);
      if (stbi_data == nullptr)
         throw std::runtime_error(stbi_failure_reason());

      data = stbi_data;
   }

   if (size_.x <= 0 || size_.y <= 0)
      throw std::runtime_error("Texture must have nonzero width and height!");

   upload(static_cast<const GLubyte*>(data));

   if (stbi_data != nullptr)
      stbi_image_free(stbi_data);
}

void Texture::upload(const GLubyte* data)
{
   glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
   glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
   glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texture_id_);
   glBindTexture(GL_TEXTURE_2D, texture_id_);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_.x, size_.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   if (glGetError() != GL_NO_ERROR)
      throw std::runtime_error("Failed to upload texture data to GPU!");
}

Texture::~Texture()
{
   if (bound_id_ == texture_id_)
      bound_id_ = 0;

   glDeleteTextures(1, &texture_id_);
}

GLuint Texture::getTextureGlId() const
{
	return texture_id_;
}

void Texture::enable() const
{
   checkUnknown();
   checkTexture();
}

void Texture::enable(GLenum mode) const
{
   checkUnknown();
   checkMode(mode);
   checkTexture();
}

void Texture::enable(GLenum mode, const glm::vec4& color) const
{
   checkUnknown();
   checkMode(mode);

   if (color != color_)
   {
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &(color.r));
      color_ = color;
   }

   checkTexture();
}

void Texture::disable() const
{
   checkUnknown();
   if (state_ == ENABLED && bound_id_ == texture_id_)
   {
      glDisable(GL_TEXTURE_2D);
      state_ = DISABLED;
   }
}

void Texture::disableAny()
{
   checkUnknown();
   if (state_ == ENABLED)
   {
      glDisable(GL_TEXTURE_2D);
      state_ = DISABLED;
   }
}

void Texture::checkUnknown()
{
   if (state_ == UNKNOWN)
   {
      state_ = DISABLED;
      
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode_);
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, &(color_.r));
   }
}

void Texture::checkTexture() const
{
   if (bound_id_ != texture_id_)
   {
      glBindTexture(GL_TEXTURE_2D, texture_id_);
      bound_id_ = texture_id_;
   }

   if (state_ == DISABLED)
   {
      glEnable(GL_TEXTURE_2D);
      state_ = ENABLED;
   }
}

void Texture::checkMode(GLenum mode)
{
   if (mode_ != mode)
   {
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
      mode_ = mode;
   }
}

} // namespace assets
} // namespace carcassonne
