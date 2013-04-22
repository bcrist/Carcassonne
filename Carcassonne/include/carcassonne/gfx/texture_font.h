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
// File: carcassonne/gfx/texture_font.h

#ifndef CARCASSONNE_GFX_TEXTURE_FONT_H_
#define CARCASSONNE_GFX_TEXTURE_FONT_H_
#include "carcassonne/_carcassonne.h"

#include <unordered_map>

#include "carcassonne/gfx/sprite.h"

namespace carcassonne {
namespace gfx {
   
class TextureFontCharacter
{
   friend class TextureFont;
public:
   TextureFontCharacter();
   TextureFontCharacter(AssetManager& asset_mgr, int font_id, unsigned int character);
   TextureFontCharacter(const TextureFontCharacter& other);
   TextureFontCharacter& operator=(const TextureFontCharacter& other);
   ~TextureFontCharacter();
   
   float getWidth() const;

   Rect getBounds() const;

   void draw() const;
   void draw(GLenum texture_mode) const;
   void draw(GLenum texture_mode, const glm::vec4& color) const;

   void init() const;

private:
   void drawBase() const;

   Sprite sprite_;
   glm::vec2 offset_;
   float width_;
   mutable GLuint display_list_;
};

class TextureFont
{
public:
   TextureFont(AssetManager& asset_mgr, const std::string& name);

   void init() const;

   void loadCharacters(unsigned int first, unsigned int count);
	
	void print(const std::string &content);
   void print(const std::string &content, GLenum texture_mode);
   void print(const std::string &content, GLenum texture_mode, const glm::vec4& color);

   float getWidth(const std::string& content);
   Rect getBounds(const std::string& content);

private:
   Rect expandRect(const Rect& rect, const Rect& other);

   AssetManager& asset_mgr_;

   int id_;
   unsigned int default_character_;

   // use an unordered map instead of a sparse vector in case 
   // we want to support unicode in the future
	std::unordered_map<unsigned int, TextureFontCharacter> characters_;

   // disable copy & assign
   TextureFont(const TextureFont&);
   void operator=(const TextureFont&);
};

} // namespace carcassonne::gfx
} // namespace carcassonne

#endif