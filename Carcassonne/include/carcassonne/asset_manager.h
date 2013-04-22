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
// File: carcassonne/asset_manager.h

#ifndef CARCASSONNE_ASSET_MANAGER_H_
#define CARCASSONNE_ASSET_MANAGER_H_
#include "carcassonne/_carcassonne.h"

#include <unordered_map>

#include "carcassonne/db/db.h"
#include "carcassonne/gfx/texture.h"
#include "carcassonne/gfx/sprite.h"
#include "carcassonne/gfx/texture_font.h"
#include "carcassonne/gfx/mesh.h"
#include "carcassonne/gui/menu.h"
#include "carcassonne/pile.h"

namespace carcassonne {

class AssetManager
{
public:
   AssetManager(const std::string& filename);

   db::DB& getDB();

   void reload();

   gfx::Texture* getTexture(const std::string& name);
   gfx::TextureFont* getTextureFont(const std::string& name);
   gfx::Mesh* getMesh(const std::string& name);
   gfx::Sprite getSprite(const std::string& name);
   Pile getTileSet(const std::string& name);

   std::unique_ptr<gui::Menu> getMenu(const std::string& name);

private:
   db::DB db_;

   std::unordered_map<std::string, std::unique_ptr<gfx::Texture> > textures_;
   std::unordered_map<std::string, gfx::Sprite> sprites_;
   std::unordered_map<std::string, std::unique_ptr<gfx::TextureFont> > fonts_;
   std::unordered_map<std::string, std::unique_ptr<gfx::Mesh> > meshes_;
   std::unordered_map<std::string, std::unique_ptr<gui::Menu> > menus_;

   AssetManager(const AssetManager&);
   void operator=(const AssetManager&);
};

} // namespace carcassonne

#endif
