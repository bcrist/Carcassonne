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
// File: carcassonne/follower.h
//
// Represents one of a player's follower tokens which can be placed on tiles
// to score points.  Followers can also have no owner player to indicate places
// where followers can be placed on tiles.

#ifndef CARCASSONNE_FOLLOWER_H_
#define CARCASSONNE_FOLLOWER_H_
#include "carcassonne/_carcassonne.h"

namespace carcassonne {

class Player;
class AssetManager;

namespace gfx {

class Mesh;

} // namespace carcassonne::gfx

class Follower
{
public:
   Follower(AssetManager& asset_mgr);
   Follower(AssetManager& asset_mgr, int id);
   Follower(const Follower& other);
   Follower(AssetManager& asset_mgr, Player& owner);
   Player* getOwner()const;
   bool isIdle()const;
   void setIdle(bool idle);

   void setOrientation(const Follower& other);
   void setPosition(const glm::vec3& position);

   void setColor(const glm::vec4& color);

   
   // called by Player::draw() if the follower is idle, or Tile::draw() (after
   // setting up the tile's local transform matrix) if the follower is not idle
   // or has no owner player.
   void draw() const;

private:
   Player* owner_;

   gfx::Mesh* mesh_;  // the mesh used to render the follower
   glm::vec4 color_;

   bool idle_;       // false if this follower is currently in use.

   

   glm::vec3 position_; // position of follower. When idle, position is absolute in the HUD camera's world space.
                        // When follower is not idle, position is relative to the tile it is placed on.
   bool farming_; // followers are rendered laying down when farming.  Ignored when idle
   glm::mat4 farming_transform_; // transforms a standing follower to a laying down follower
   float rotation_;  // rotation around y-axis.  Ignored when idle

   void operator=(const Follower&);
};

} // namespace carcassonne

#endif
