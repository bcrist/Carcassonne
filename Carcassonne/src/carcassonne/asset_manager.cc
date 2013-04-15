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
// File: carcassonne/asset_manager.cc

#include "carcassonne/asset_manager.h"

namespace carcassonne {

AssetManager::AssetManager(const std::string& filename)
   : db_(filename)
{
}

db::DB& AssetManager::getDB()
{
   return db_;
}

gfx::Texture* AssetManager::getTexture(const std::string& name)
{
   std::unique_ptr<gfx::Texture>& ptr = textures_[name];

   if (!ptr)
   {
      try
      {
         ptr.reset(new gfx::Texture(db_, name));
      }
      catch (const std::runtime_error& err)
      {
         std::cerr << "Failed to load texture \"" << name << "\": " << err.what();
      }
   }

   return ptr.get();
}

const gfx::Sprite& AssetManager::getSprite(const std::string& name)
{
   auto i(sprites_.find(name));
   if (i == sprites_.end())
   {
      try
      {
         sprites_.insert(std::make_pair(name, Sprite(*this, name)));
      }
      catch (std::runtime_error& err)
      {
         std::cerr << "Failed to load sprite \"" << name << "\": " << err.what();
         
      }
   }
   return i->second;
}

} // namespace carcassonne
