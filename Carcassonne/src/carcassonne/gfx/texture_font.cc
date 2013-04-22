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
// File: carcassonne/gfx/texture_font.cc

#include "carcassonne/gfx/texture_font.h"

#include "carcassonne/asset_manager.h"
#include "carcassonne/db/stmt.h"

namespace carcassonne {
namespace gfx {

TextureFontCharacter::TextureFontCharacter()
   : width_(0),
     display_list_(0)
{
}

TextureFontCharacter::TextureFontCharacter(AssetManager& asset_mgr, int font_id, unsigned int character)
   : width_(0),
     display_list_(0)
{
   db::DB& db(asset_mgr.getDB());

   db::Stmt s(db, "SELECT sprite, offset_x, offset_y, width FROM cc_texfont_characters WHERE font_id = ? AND character = ?");
   s.bind(1, font_id);
   s.bind(2, static_cast<int>(character));
   if (s.step())
   {
      sprite_ = asset_mgr.getSprite(s.getText(0));
      offset_ = glm::vec2(static_cast<float>(s.getDouble(1)), static_cast<float>(s.getDouble(2)));
      width_ = static_cast<float>(s.getDouble(3));
   }
   else
      std::cerr << "Warning: Failed to load character " << character << " from font " << font_id << "!" << std::endl;
}

TextureFontCharacter::TextureFontCharacter(const TextureFontCharacter& other)
   : sprite_(other.sprite_),
     offset_(other.offset_),
     width_(other.width_),
     display_list_(0)
{
}

TextureFontCharacter& TextureFontCharacter::operator=(const TextureFontCharacter& other)
{
   sprite_ = other.sprite_;
   offset_ = other.offset_;
   width_ = other.width_;

   if (display_list_ != 0)
   {
      glDeleteLists(display_list_, 1);
      display_list_ = 0;
   }
}

TextureFontCharacter::~TextureFontCharacter()
{
   if (display_list_ != 0)
      glDeleteLists(display_list_, 1);
}

float TextureFontCharacter::getWidth() const
{
   return width_;
}

Rect TextureFontCharacter::getBounds() const
{
   return Rect(offset_ + glm::vec2(width_ * 0.5f, 0) - sprite_.texture_coords.size * 0.5f, sprite_.texture_coords.size);
}

void TextureFontCharacter::draw() const
{
   if (sprite_.texture)
      sprite_.texture->enable();
   else
      std::cerr << "Warning: drawing character with no texture specified!" << std::endl;
   
   drawBase();
}

void TextureFontCharacter::draw(GLenum texture_mode) const
{
   if (sprite_.texture)
      sprite_.texture->enable();
   else
      std::cerr << "Warning: drawing character with no texture specified!" << std::endl;
   
   drawBase();
}

void TextureFontCharacter::draw(GLenum texture_mode, const glm::vec4& color) const
{
   if (sprite_.texture)
      sprite_.texture->enable();
   else
      std::cerr << "Warning: drawing character with no texture specified!" << std::endl;
   
   drawBase();
}

void TextureFontCharacter::drawBase() const
{
   if (display_list_ > 0)
      glCallList(display_list_);
   else
   {
      Rect bounds(getBounds());

      display_list_ = glGenLists(1);
      glNewList(display_list_, GL_COMPILE_AND_EXECUTE);

      glBegin(GL_QUADS);

      glTexCoord2f(sprite_.texture_coords.left(),  sprite_.texture_coords.top());    glVertex2f(bounds.left(), bounds.top());
      glTexCoord2f(sprite_.texture_coords.left(),  sprite_.texture_coords.bottom()); glVertex2f(bounds.left(), bounds.bottom());
      glTexCoord2f(sprite_.texture_coords.right(), sprite_.texture_coords.bottom()); glVertex2f(bounds.right(), bounds.bottom());
      glTexCoord2f(sprite_.texture_coords.right(), sprite_.texture_coords.top());    glVertex2f(bounds.right(), bounds.top());

      glEnd();

      glTranslatef(width_, 0, 0);

      glEndList();
   }
}

void TextureFontCharacter::init() const
{
   if (display_list_ != 0)
   {
      glDeleteLists(display_list_, 1);
      display_list_ = 0;
   }
}


TextureFont::TextureFont(AssetManager& asset_mgr, const std::string& name)
   : asset_mgr_(asset_mgr)
{
   db::DB& db(asset_mgr.getDB());

   unsigned int preload_start = 0;
   unsigned int preload_count = 0;

   db::Stmt s(db, "SELECT id, default_character, preload_start, preload_count FROM cc_texfonts WHERE name = ?");
   s.bind(1, name);
   if (s.step())
   {
      id_ = s.getInt(0);
      default_character_ = static_cast<unsigned int>(s.getInt(1));
      preload_start = static_cast<unsigned int>(s.getInt(2));
      preload_count = static_cast<unsigned int>(s.getInt(3));
   }
   else
      throw std::runtime_error("Texture font not found!");

   db::Stmt sc(db, "SELECT character FROM cc_texfont_characters WHERE font_id = ?");
   sc.bind(1, id_);
   while (sc.step())
      characters_[static_cast<unsigned int>(sc.getInt(0))] = TextureFontCharacter();

   loadCharacters(preload_start, preload_count);
}

void TextureFont::init() const
{
   for (auto i(characters_.begin()), end(characters_.end()); i != end; ++i)
      i->second.init();
}

void TextureFont::loadCharacters(unsigned int first, unsigned int count)
{
   for (unsigned int i = first; i < first + count; ++i)
   {
      auto j(characters_.find(i));
      if (j == characters_.end())
         continue;

      TextureFontCharacter& tfc = j->second;

      if (tfc.sprite_.texture == nullptr)
         tfc = TextureFontCharacter(asset_mgr_, id_, i);
   }
}

void TextureFont::print(const std::string& content)
{
   glPushMatrix();

   for (auto i(content.begin()), end(content.end()); i != end; ++i)
   {
      auto j(characters_.find(*i));
      if (j == characters_.end())
      {
         j = characters_.find(default_character_);
         if (j == characters_.end())
            continue;
      }

      TextureFontCharacter& tfc = j->second;

      if (tfc.sprite_.texture == nullptr)
         tfc = TextureFontCharacter(asset_mgr_, id_, j->first);

      tfc.draw();
   }

   glPopMatrix();
}

void TextureFont::print(const std::string& content, GLenum texture_mode)
{
   glPushMatrix();

   for (auto i(content.begin()), end(content.end()); i != end; ++i)
   {
      auto j(characters_.find(*i));
      if (j == characters_.end())
      {
         j = characters_.find(default_character_);
         if (j == characters_.end())
            continue;
      }

      TextureFontCharacter& tfc = j->second;

      if (tfc.sprite_.texture == nullptr)
         tfc = TextureFontCharacter(asset_mgr_, id_, j->first);

      tfc.draw(texture_mode);
   }

   glPopMatrix();
}

void TextureFont::print(const std::string& content, GLenum texture_mode, const glm::vec4& color)
{
   glPushMatrix();

   for (auto i(content.begin()), end(content.end()); i != end; ++i)
   {
      auto j(characters_.find(*i));
      if (j == characters_.end())
      {
         j = characters_.find(default_character_);
         if (j == characters_.end())
            continue;
      }

      TextureFontCharacter& tfc = j->second;

      if (tfc.sprite_.texture == nullptr)
         tfc = TextureFontCharacter(asset_mgr_, id_, j->first);

      tfc.draw(texture_mode, color);
   }

   glPopMatrix();
}

float TextureFont::getWidth(const std::string& content)
{
   float width = 0;

   for (auto i(content.begin()), end(content.end()); i != end; ++i)
   {
      auto j(characters_.find(*i));
      if (j == characters_.end())
      {
         j = characters_.find(default_character_);
         if (j == characters_.end())
            continue;
      }

      TextureFontCharacter& tfc = j->second;

      if (tfc.sprite_.texture == nullptr)
         tfc = TextureFontCharacter(asset_mgr_, id_, j->first);

      width += tfc.width_;
   }

   return width;
}

Rect TextureFont::getBounds(const std::string& content)
{
   Rect bounds;

   for (auto i(content.begin()), end(content.end()); i != end; ++i)
   {
      auto j(characters_.find(*i));
      if (j == characters_.end())
      {
         j = characters_.find(default_character_);
         if (j == characters_.end())
            continue;
      }

      TextureFontCharacter& tfc = j->second;

      if (tfc.sprite_.texture == nullptr)
         tfc = TextureFontCharacter(asset_mgr_, id_, j->first);

      expandRect(bounds, tfc.getBounds());
   }

   return bounds;
}

Rect TextureFont::expandRect(const Rect& rect, const Rect& other)
{
   float left = std::min(rect.left(), other.left());
   float right = std::max(rect.right(), other.right());
   float top = std::min(rect.top(), other.top());
   float bottom = std::max(rect.bottom(), other.bottom());

   return Rect(left, top, right - left, bottom - top);
}

} // namespace carcassonne::gfx
} // namespace carcassonne
