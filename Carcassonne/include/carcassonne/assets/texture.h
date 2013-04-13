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
// File: carcassonne/assets/texture.h
//
// TODO:
//  - Allow different filtering modes
//  - Allow mipmap generation
//     - option to skip levels based on texture quality in GraphicsConfiguration
//  - GL_TEXTURE_BASE_LEVEL, GL_TEXTURE_MAX_LEVEL, 
//    GL_TEXTURE_MIN_LOD, GL_TEXTURE_MAX_LOD,
//    GL_TEXTURE_LOD_BIAS
//  - Texture prioritization
//  - OpenGL texture compression & hints based on quality in GraphicsConfiguration
//  - Anisotropic filtering
//  - 1D/3D/Cubemap textures
//  - more database formats
//  - database texture data compression (zlib)

#ifndef CARCASSONNE_ASSETS_TEXTURE_H_
#define CARCASSONNE_ASSETS_TEXTURE_H_
#include "carcassonne/_carcassonne.h"

#include <string>
#include <glm/glm.hpp>
#include <SFML/OpenGL.hpp>

#include "carcassonne/db/db.h"

namespace carcassonne {
namespace assets {

class Texture
{
public:
   Texture(const GLubyte* data, const glm::ivec2& size);
   Texture(db::DB& db, const std::string& name);
   ~Texture();

   GLuint getTextureGlId() const;

   void enable() const;
   void enable(GLenum mode) const;
   void enable(GLenum mode, const glm::vec4& color) const;
   void disable() const;

   static void disableAny();

private:
   void upload(const GLubyte* data);

   static void checkUnknown();
   static void checkMode(GLenum mode);
   void checkTexture() const;
   
   enum State { UNKNOWN, DISABLED, ENABLED };

   static State state_;
   static GLuint bound_id_;
   static GLenum mode_;
   static glm::vec4 color_;

   glm::ivec2 size_;
   GLuint texture_id_;

   Texture(const Texture&);
   void operator=(const Texture&);
};

} // namespace assets
} // namespace carcassonne

#endif
