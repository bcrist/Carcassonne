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
// Author: Benjamin Crist / Josh Douglas
// File: carcassonne/follower.h
//
// Represents one of a player's follower tokens which can be placed on tiles
// to score points.  Followers can also have no owner player to indicate places
// where followers can be placed on tiles.

#include "carcassonne/follower.h"

#include <glm/gtc/matrix_transform.hpp>

#include "carcassonne/player.h"
#include "carcassonne/asset_manager.h"
#include "carcassonne/gfx/mesh.h"
#include "carcassonne/db/db.h"
#include "carcassonne/db/stmt.h"

namespace carcassonne {

Follower::Follower(AssetManager& asset_mgr)
   : owner_(nullptr),
     mesh_(asset_mgr.getMesh("std-follower")),
     color_(1,1,1,1),
     idle_(false),
     farming_(false),
     rotation_(0)
{
   // half follower height: 0.1775
   // half follower depth: 0.115

   farming_transform_ = glm::translate(glm::rotate(glm::translate(farming_transform_,
      glm::vec3(0.0f, -0.1775f, 0.0f)),
      -90.0f, glm::vec3(1.0f, 0.0f, 0.0f)),
      glm::vec3(0.0f, 0.115f, 0.0f));
}

// load from db
Follower::Follower(AssetManager& asset_mgr, int feature_id)
   : owner_(nullptr),
     mesh_(asset_mgr.getMesh("std-follower")),
     color_(1,1,1,1),
     idle_(false)
{
   db::DB& db = asset_mgr.getDB();

   db::Stmt s(db, "SELECT follower_orientation, follower_x, follower_z, follower_r "
                  "FROM cc_tile_features "
                  "WHERE id = ?");
   s.bind(1, feature_id);
   if (s.step())
   {
      farming_ = s.getInt(0) != 0;
      position_.x = static_cast<float>(s.getDouble(1));
      position_.z = static_cast<float>(s.getDouble(2));
      rotation_ = static_cast<float>(s.getDouble(3));
   }
   else
      throw std::runtime_error("Could not find feature to load follower data!");

   farming_transform_ = glm::translate(glm::rotate(glm::translate(farming_transform_,
      glm::vec3(0.0f, -0.1775f, 0.0f)),
      -90.0f, glm::vec3(1.0f, 0.0f, 0.0f)),
      glm::vec3(0.0f, 0.115f, 0.0f));
}

// copies a follower (except owning player)
Follower::Follower(const Follower& other)
   : owner_(nullptr),
     mesh_(other.mesh_),
     color_(other.color_),
     idle_(other.idle_),
     position_(other.position_),
     farming_(other.farming_),
     farming_transform_(other.farming_transform_),
     rotation_(other.rotation_)
{
}

Follower::Follower(AssetManager& asset_mgr, Player& owner)
   : owner_(&owner),
     mesh_(asset_mgr.getMesh("std-follower")),
     color_(owner.getColor()),
     idle_(true),
     farming_(false),
     rotation_(0)
{
   farming_transform_ = glm::translate(glm::rotate(glm::translate(farming_transform_,
      glm::vec3(0.0f, -0.1775f, 0.0f)),
      -90.0f, glm::vec3(1.0f, 0.0f, 0.0f)),
      glm::vec3(0.0f, 0.115f, 0.0f));
}

Player* Follower::getOwner()const
{
   return owner_;
}

void Follower::setColor(const glm::vec4& color)
{
   color_ = color;
}

void Follower::draw()const
{
   glPushMatrix();

   glTranslatef(position_.x, position_.y, position_.z);
   glRotatef(rotation_, 0, 1, 0);

   if (farming_)
      glMultMatrixf(glm::value_ptr(farming_transform_));
   
   glColor4fv(glm::value_ptr(color_));
   bool disable_depth_write = color_.a < 1;

   if (disable_depth_write)
      glDepthMask(false);

   if (mesh_)
      mesh_->draw(GL_MODULATE);

   if (disable_depth_write)
      glDepthMask(true);

   glPopMatrix();
}

bool Follower::isIdle()const
{
   return idle_;
}

void Follower::setIdle(bool idle)
{
   idle_ = idle;
}

void Follower::setOrientation(const Follower& other)
{
   position_ = other.position_;
   farming_ = other.farming_;
   rotation_ = other.rotation_;
}

void Follower::setPosition(const glm::vec3& position)
{
   position_ = position;
}


}// namespace carcassonne
