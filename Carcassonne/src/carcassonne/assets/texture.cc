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
#include "stb_image.h"

namespace bmc {

Texture::State Texture::state_(UNKNOWN);
GLuint Texture::bound_id_(0);
GLenum Texture::mode_(GL_MODULATE);
glm::vec4 Texture::color_(0,0,0,0);

Texture::Texture(const std::string& filename)
{
   int comps;
   GLubyte* data = stbi_load(filename.c_str(), &width_, &height_, &comps, 4);

   if (data == nullptr)
      throw std::exception("Could not load texture file!");

   upload(data);
}

Texture::Texture(const char* filename)
{
   int comps;
   GLubyte* data = stbi_load(filename, &width_, &height_, &comps, 4);

   if (data == nullptr)
      throw std::exception("Could not load texture file!");

   upload(data);
}

Texture::Texture(const GLubyte* data, int width, int height)
   : width_(width),
     height_(height)
{
   assert(data != nullptr);
   assert(width > 0);
   assert(height > 0);
   upload(data);
}

void Texture::upload(const GLubyte* data)
{

   glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
   glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
   glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texture_id_);
   glBindTexture(GL_TEXTURE_2D, texture_id_);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   if (glGetError() != GL_NO_ERROR)
      throw std::exception("Failed to upload texture data to GPU!");
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

} // namespace bmc
