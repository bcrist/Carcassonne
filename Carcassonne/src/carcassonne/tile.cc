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
// File: carcassonne/tile.cc
//
// Represents a single game tile which can be placed in a draw pile or on the
// board.

#include "carcassonne/tile.h"

#include <ctime>
#include <glm/gtc/matrix_transform.hpp>

#include "carcassonne/asset_manager.h"
#include "carcassonne/db/db.h"
#include "carcassonne/db/stmt.h"

namespace carcassonne {

#pragma region TileEdge

TileEdge::TileEdge()
   : type(TYPE_FARM),
     open(true),
     farm(nullptr),
     cw_farm(nullptr),
	  ccw_farm(nullptr)
{
}

TileEdge::TileEdge(features::Farm* farm)
	: type(TYPE_FARM),
	  open(true),
	  farm(farm),
	  cw_farm(nullptr),
	  ccw_farm(nullptr)
{
}

TileEdge::TileEdge(features::Road* road, features::Farm* cw_farm, features::Farm* ccw_farm)
	: type(TYPE_ROAD),
	  open(true),
	  road(road),
	  cw_farm(cw_farm),
	  ccw_farm(ccw_farm)
{
}

TileEdge::TileEdge(features::City* city)
	: type(TYPE_CITY),
	  open(true),
	  city(city),
	  cw_farm(nullptr),
	  ccw_farm(nullptr)
{
}

#pragma endregion

// used to randomize starting rotation of tiles
std::mt19937 Tile::prng_(static_cast<std::mt19937::result_type>(time(nullptr)));

// Constructs a tile of one of the TYPE_EMPTY_* types
Tile::Tile(AssetManager& asset_mgr, Type type)
	: type_(type),
     color_(1,1,1,1),
	  mesh_(asset_mgr.getMesh("std-tile")),
     texture_(nullptr),
     rotation_(ROTATION_NONE),
     transforms_valid_(0)
{
   assert(type != TYPE_PLACED && type != TYPE_FLOATING);
   setType(type);
}

// Load tile from database
Tile::Tile(AssetManager& asset_mgr, const std::string& name)
   : type_(TYPE_FLOATING),
     color_(1,1,1,1),
     mesh_(asset_mgr.getMesh("std-tile")),
     rotation_(static_cast<Rotation>(prng_() % 4)),
     transforms_valid_(0)
{
   db::DB& db = asset_mgr.getDB();


   db::Stmt sf(db, "SELECT type, pennants, adjacent1, adjacent2, adjacent3, adjacent4 "
                   "FROM cc_tile_features "
                   "WHERE id = ?");

   db::Stmt s(db, "SELECT texture, cloister, "
                  "north, north_cw, north_ccw, "
                  "east, east_cw, east_ccw, "
                  "south, south_cw, south_ccw, "
                  "west, west_cw, west_ccw "
                  "FROM cc_tiles "
                  "WHERE name = ?");
   s.bind(1, name);
   if (s.step())
   {
      texture_ = asset_mgr.getTexture(s.getText(0));

      int cloister_id = s.getInt(1);
      if (cloister_id > 0)
         cloister_ = std::shared_ptr<features::Feature>(new features::Cloister(asset_mgr, cloister_id, *this));

      std::vector<FeatureRef> features;

      for (int i = 0; i < 4; ++i)
      {
         int index = 2 + 3 * i;  // statement column index
         int id = s.getInt(index);

         TileEdge& edge = edges_[i];

         FeatureRef ref = getFeature(asset_mgr, sf, features, id);
         edge.type = ref.type;
         switch (edge.type)
         {
            case TileEdge::TYPE_CITY:
               edge.city = ref.city;
               break;

            case TileEdge::TYPE_FARM:
               edge.farm = ref.farm;
               break;

            case TileEdge::TYPE_ROAD:
               {
                  edge.road = ref.road;
                  FeatureRef cw_ref = getFeature(asset_mgr, sf, features, s.getInt(index + 1));
                  if (cw_ref.type != TileEdge::TYPE_FARM)
                     throw std::runtime_error("Unexpected feature type found!  Expected farm!");
                  edge.cw_farm = cw_ref.farm;

                  FeatureRef ccw_ref = getFeature(asset_mgr, sf, features, s.getInt(index + 2));
                  if (ccw_ref.type != TileEdge::TYPE_FARM)
                     throw std::runtime_error("Unexpected feature type found!  Expected farm!");
                  edge.ccw_farm = ccw_ref.farm;
               }
               break;

            default:
               break;
         }
      }
   }
   else
      throw std::runtime_error("Tile not found!");
}

Tile::FeatureRef Tile::getFeature(AssetManager& asset_mgr, db::Stmt& sf, std::vector<FeatureRef>& features, int id)
{
   FeatureRef ref;
   ref.id = id;

   auto i(std::find(features.begin(), features.end(), ref));
   if (i != features.end())
      return *i;

   sf.bind(1, id);
   if (!sf.step())
      throw std::runtime_error("Tile feature not found!");

   ref.type = static_cast<TileEdge::Type>(sf.getInt(0));
   switch (ref.type)
   {
      case TileEdge::TYPE_CITY:
         ref.city = new features::City(asset_mgr, id, sf.getInt(1), *this);
         cities_.push_back(std::shared_ptr<features::Feature>(ref.city));
         features.push_back(ref);
         break;

      case TileEdge::TYPE_FARM:
         {
            ref.farm = new features::Farm(asset_mgr, id, *this);
            farms_.push_back(std::shared_ptr<features::Feature>(ref.farm));
            features.push_back(ref);

            int adjacent[4] = { sf.getInt(2), sf.getInt(3), sf.getInt(4), sf.getInt(5) };
            sf.reset();

            for (int i = 0; i < 4; ++i)
            {
               if (adjacent[i] > 0)
               {
                  FeatureRef c = getFeature(asset_mgr, sf, features, adjacent[i]);
                  if (c.type == TileEdge::TYPE_CITY)
                     ref.farm->addAdjacentCity(*c.city);
               }
            }
         }
         break;

      case TileEdge::TYPE_ROAD:
         ref.road = new features::Road(asset_mgr, id, *this);
         roads_.push_back(std::shared_ptr<features::Feature>(ref.road));
         features.push_back(ref);
         break;

      default:
         throw std::runtime_error("Unrecognized feature type!");
   }

   sf.reset();
   return ref;
}


// Copy another tile (does not share feature objects)
Tile::Tile(const Tile& other)
   : type_(other.type_),
     color_(other.color_),
     mesh_(other.mesh_),
     texture_(other.texture_),
     rotation_(ROTATION_NONE),
     transforms_valid_(0)
{
   for (int i = 0; i < 4; ++i)
      edges_[i] = other.edges_[i];

   for (auto i(other.cities_.begin()), end(other.cities_.end()); i != end; ++i)
   {
      const features::City& city = static_cast<const features::City&>(**i);
      features::City* new_city = new features::City(city, *this);

      cities_.push_back(std::shared_ptr<features::Feature>(new_city));

      for (int i = 0; i < 4; ++i)
      {
         TileEdge& edge = edges_[i];
         if (edge.type == TileEdge::TYPE_CITY && edge.city == &city)
            edge.city = new_city;
      }
   }

   for (auto i(other.roads_.begin()), end(other.roads_.end()); i != end; ++i)
   {
      const features::Road& road = static_cast<const features::Road&>(**i);
      features::Road* new_road = new features::Road(road, *this);

      roads_.push_back(std::shared_ptr<features::Feature>(new_road));

      for (int i = 0; i < 4; ++i)
      {
         TileEdge& edge = edges_[i];
         if (edge.type == TileEdge::TYPE_ROAD && edge.road == &road)
            edge.road = new_road;
      }
   }

   for (auto i(other.farms_.begin()), end(other.farms_.end()); i != end; ++i)
   {
      const features::Farm& farm = static_cast<const features::Farm&>(**i);
      features::Farm* new_farm = new features::Farm(farm, *this);

      farms_.push_back(std::shared_ptr<features::Feature>(new_farm));

      for (auto i(other.cities_.begin()), end(other.cities_.end()); i != end; ++i)
      {
         const features::City& city = static_cast<const features::City&>(**i);
         features::City& new_city = static_cast<features::City&>(
            **(cities_.begin() + (i - other.cities_.begin())));

         new_farm->replaceCity(city, new_city);
      }

      for (int i = 0; i < 4; ++i)
      {
         TileEdge& edge = edges_[i];
         if (edge.type == TileEdge::TYPE_ROAD)
         {
            if (edge.cw_farm == &farm)
               edge.cw_farm = new_farm;

            if (edge.ccw_farm == &farm)
               edge.ccw_farm = new_farm;
         }
         else if (edge.type == TileEdge::TYPE_FARM && edge.farm == &farm)
            edge.farm = new_farm;
      }
   }

   if (other.cloister_)
   {
      const features::Cloister& cloister = static_cast<const features::Cloister&>(*other.cloister_);
      features::Cloister* new_cloister = new features::Cloister(cloister, *this);
      cloister_.reset(new_cloister);
   }
}


// Sets the tile's type.  A TYPE_EMPTY_* tile can only be set to any of the
// other TYPE_EMPTY_* types.  A TYPE_PLACED tile can't be changed to any
// other type.  A TYPE_FLOATING tile can only be changed to TYPE_PLACED.
void Tile::setType(Type type)
{
   switch(type_)
   {
      case TYPE_EMPTY_PLACEABLE:
      case TYPE_EMPTY_PLACEABLE_IF_ROTATED:
      case TYPE_EMPTY_NOT_PLACEABLE:
         if (type == TYPE_FLOATING || type == TYPE_PLACED)
            break;
         else
            type_ = type;

         if (type == TYPE_EMPTY_PLACEABLE)
            color_ = glm::vec4(0.4, 1, 0.4, 0.2);
         else if (type == TYPE_EMPTY_PLACEABLE_IF_ROTATED)
            color_ = glm::vec4(1, 1, 0.4, 0.2);
         else
            color_ = glm::vec4(1, 0.4, 0.4, 0.2);
         break;

      case TYPE_FLOATING:
         if (type != TYPE_PLACED)
            break;
         type_ = type;
         break;

      default:
         break;
   }
         
}

Tile::Type Tile::getType()const
{
   return type_;
}

// Rotates the tile if it is a TYPE_FLOATING tile
void Tile::rotateClockwise()
{
   if (type_ != TYPE_FLOATING)
      return;
   rotation_ = static_cast<Rotation>((static_cast<int>(rotation_) + 1) % 4);
   transforms_valid_ = 0;
}

void Tile::rotateCounterclockwise()
{
   if (type_ != TYPE_FLOATING)
      return;
   rotation_ = static_cast<Rotation>((static_cast<int>(rotation_) + 3) % 4);
   transforms_valid_ = 0;
}

Tile::Rotation Tile::getRotation() const
{
   return rotation_;
}

void Tile::setPosition(const glm::vec3& position)
{
   position_ = position;
   transforms_valid_ = 0;
}

const glm::vec3& Tile::getPosition() const
{
   return position_;
}

glm::vec3 Tile::localToWorld(const glm::vec3& local_coords) const
{
   if (transforms_valid_ < 2)
      calculateInverseTransform();

   return glm::vec3(inv_transform_ * glm::vec4(local_coords, 1.0f));
}

glm::vec3 Tile::worldToLocal(const glm::vec3& world_coords) const
{
   if (transforms_valid_ < 1)
      calculateTransform();

   return glm::vec3(transform_ * glm::vec4(world_coords, 1.0f));
}

void Tile::calculateTransform() const
{
   float angle = 90.0f * static_cast<int>(rotation_);
   transform_ = glm::translate(glm::rotate(glm::mat4(), angle, glm::vec3(0, 1, 0)), -position_);
   transforms_valid_ = 1;
}

void Tile::calculateInverseTransform() const
{
   if (transforms_valid_ < 1)
      calculateTransform();

   inv_transform_ = glm::inverse(transform_);
   transforms_valid_ = 2;
}


// Returns the type of features which currently exist on the requested side.
const TileEdge& Tile::getEdge(Side side) const
{
   return edges_[(static_cast<int>(side) + 4 - static_cast<int>(rotation_)) % 4];
}

TileEdge& Tile::getEdge_(Side side)
{
   return edges_[(static_cast<int>(side) + 4 - static_cast<int>(rotation_)) % 4];
}

size_t Tile::getFeatureCount() const
{
   return cities_.size() + roads_.size() + farms_.size() + (cloister_ ? 1 : 0);
}

std::weak_ptr<features::Feature> Tile::getFeature(size_t index)
{
   if (index < cities_.size())
      return std::weak_ptr<features::Feature>(cities_[index]);

   index -= cities_.size();
   if (index < roads_.size())
      return std::weak_ptr<features::Feature>(roads_[index]);

   index -= roads_.size();
   if (index < farms_.size())
      return std::weak_ptr<features::Feature>(farms_[index]);

   assert(index == farms_.size());

   return std::weak_ptr<features::Feature>(cloister_);
}

// called when a tile is placed
// should be called on (up to) all four sides of the new tile
void Tile::closeSide(Side side, Tile* new_neighbor)
{
   if (new_neighbor == nullptr || new_neighbor->type_ != TYPE_PLACED)
      return;

   Side neighbor_side = static_cast<Side>((side + 2) % 4);
   TileEdge& neighbor_edge = new_neighbor->getEdge_(neighbor_side);
   TileEdge& edge = getEdge_(side);

   edge.open = false;
   neighbor_edge.open = false;

   if (neighbor_edge.type != edge.type)
   {
      std::cerr << "Warning: edge mismatch!" << std::endl;
      return;
   }

   switch (edge.type)
   {
      case TileEdge::TYPE_FARM:
         neighbor_edge.farm->join(*edge.farm);
         break;

      case TileEdge::TYPE_ROAD:
         neighbor_edge.road->join(*edge.road);
         neighbor_edge.cw_farm->join(*edge.ccw_farm);
         neighbor_edge.ccw_farm->join(*edge.cw_farm);
         break;

      case TileEdge::TYPE_CITY:
         neighbor_edge.city->join(*edge.city);
         break;

      default:
         break;
   }

   if (cloister_)
      static_cast<features::Cloister&>(*cloister_).addTile(*new_neighbor);

   if (new_neighbor->cloister_)
   {
      static_cast<features::Cloister&>(*new_neighbor->cloister_).addTile(*this);
      new_neighbor->checkForCompleteCloister();
   }
}

void Tile::closeDiagonal(Tile* new_diagonal_neighbor)
{
   if (new_diagonal_neighbor == nullptr || new_diagonal_neighbor->type_ != TYPE_PLACED)
      return;

   if (cloister_)
      static_cast<features::Cloister&>(*cloister_).addTile(*new_diagonal_neighbor);

   if (new_diagonal_neighbor->cloister_)
   {
      static_cast<features::Cloister&>(*new_diagonal_neighbor->cloister_).addTile(*this);
      new_diagonal_neighbor->checkForCompleteCloister();
   }
}

void Tile::checkForCompleteFeatures()
{
   for (auto i(cities_.begin()), end(cities_.end()); i!= end; ++i)
   {
      features::Feature& feature = **i;
      if (feature.isComplete())
         feature.score();
   }
   /* We know farms are never complete until the end of the game, so we don't need:
   for (auto i(farms_.begin()), end(farms_.end()); i!= end; ++i)
   {
      features::Feature& feature = **i;
      if (feature.isComplete())
         feature.score();
   }
   */

   for (auto i(roads_.begin()), end(roads_.end()); i!= end; ++i)
   {
      features::Feature& feature = **i;
      if (feature.isComplete())
         feature.score();
   }

   checkForCompleteCloister();
}

void Tile::checkForCompleteCloister()
{
   if (cloister_ && cloister_->isComplete())
      cloister_->score();
}

void Tile::draw() const
{
   glPushMatrix();
   glTranslatef(position_.x, position_.y, position_.z);
   /*
   gfx::Texture::disableAny();
   glPointSize(8);
   glBegin(GL_POINTS);

   switch(getEdge(SIDE_NORTH).type)
   {
   case TileEdge::TYPE_CITY: glColor3f(1,1,0); break;
   case TileEdge::TYPE_FARM: glColor3f(0,1,0); break;
   case TileEdge::TYPE_ROAD: glColor3f(0,0,0); break;
   }
   glVertex3f(0.4, 0.1, 0);

   switch(getEdge(SIDE_EAST).type)
   {
   case TileEdge::TYPE_CITY: glColor3f(1,1,0); break;
   case TileEdge::TYPE_FARM: glColor3f(0,1,0); break;
   case TileEdge::TYPE_ROAD: glColor3f(0,0,0); break;
   }
   glVertex3f(0, 0.1, 0.4);
   
   switch(getEdge(SIDE_SOUTH).type)
   {
   case TileEdge::TYPE_CITY: glColor3f(1,1,0); break;
   case TileEdge::TYPE_FARM: glColor3f(0,1,0); break;
   case TileEdge::TYPE_ROAD: glColor3f(0,0,0); break;
   }
   glVertex3f(-0.4, 0.1, 0);
   
   switch(getEdge(SIDE_WEST).type)
   {
   case TileEdge::TYPE_CITY: glColor3f(1,1,0); break;
   case TileEdge::TYPE_FARM: glColor3f(0,1,0); break;
   case TileEdge::TYPE_ROAD: glColor3f(0,0,0); break;
   }
   glVertex3f(0, 0.1, -0.4);
   glEnd();*/



   float angle = -90.0f * static_cast<int>(rotation_);
   glRotatef(angle, 0, 1, 0);

   gfx::Texture::disableAny();
   glDisable(GL_LIGHTING);
   glBegin(GL_LINES);
   glColor3f(1,0,0);
   glVertex3f(0,0,0);glVertex3f(0.5,0,0);
   glColor3f(0,0,1);
   glVertex3f(0,0,0);glVertex3f(0,0,0.5);
   glEnd();
   glEnable(GL_LIGHTING);

   glColor4fv(glm::value_ptr(color_));
   bool disable_depth_write = color_.a < 1;

   if (texture_)
      texture_->enable(GL_MODULATE);
   else
      gfx::Texture::disableAny();

   if (disable_depth_write)
      glDepthMask(false);

   if (mesh_)
      mesh_->drawBase();

   if (disable_depth_write)
      glDepthMask(true);




   glPopMatrix();
}

void Tile::drawPlaceholders() const
{
   glPushMatrix();
   glTranslatef(position_.x, position_.y, position_.z);
   float angle = -90.0f * static_cast<int>(rotation_);
   glRotatef(angle, 0, 1, 0);

   for (auto i(cities_.begin()), end(cities_.end()); i!= end; ++i)
      (*i)->drawPlaceholder();

   for (auto i(farms_.begin()), end(farms_.end()); i!= end; ++i)
      (*i)->drawPlaceholder();

   for (auto i(roads_.begin()), end(roads_.end()); i!= end; ++i)
      (*i)->drawPlaceholder();

   if (cloister_)
      cloister_->drawPlaceholder();

   glPopMatrix();
}

void Tile::setPlaceholderColor(const glm::vec4& color)
{
   for (auto i(cities_.begin()), end(cities_.end()); i!= end; ++i)
      (*i)->setPlaceholderColor(color);

   for (auto i(farms_.begin()), end(farms_.end()); i!= end; ++i)
      (*i)->setPlaceholderColor(color);

   for (auto i(roads_.begin()), end(roads_.end()); i!= end; ++i)
      (*i)->setPlaceholderColor(color);

   if (cloister_)
      cloister_->setPlaceholderColor(color);
}

void Tile::replaceCity(const features::City& old_city, features::City& new_city)
{
   for (auto i(cities_.begin()), end(cities_.end()); i!= end; ++i)
   {
      std::shared_ptr<features::Feature>& ptr = *i;

      if (&old_city == ptr.get())
         ptr = new_city.shared_from_this();
   }

   for (auto i(farms_.begin()), end(farms_.end()); i!= end; ++i)
   {
      std::shared_ptr<features::Feature>& ptr = *i;
      features::Farm& farm = static_cast<features::Farm&>(*ptr);

      farm.replaceCity(old_city, new_city);
   }

   for (int i = 0; i < 4; ++i)
   {
      TileEdge& edge = edges_[i];
      if (edge.type == TileEdge::TYPE_CITY && edge.city == &old_city)
         edge.city = &new_city;
   }
}

void Tile::replaceFarm(const features::Farm& old_farm, features::Farm& new_farm)
{
   for (auto i(farms_.begin()), end(farms_.end()); i!= end; ++i)
   {
      std::shared_ptr<features::Feature>& ptr = *i;

      if (&old_farm == ptr.get())
         ptr = new_farm.shared_from_this();
   }

   for (int i = 0; i < 4; ++i)
   {
      TileEdge& edge = edges_[i];
      if (edge.type == TileEdge::TYPE_FARM && edge.farm == &old_farm)
         edge.farm = &new_farm;

      else if (edge.type == TileEdge::TYPE_ROAD)
      {
         if (edge.cw_farm == &old_farm)
            edge.cw_farm = &new_farm;

         if (edge.ccw_farm == &old_farm)
            edge.ccw_farm = &new_farm;
      }
   }
}

void Tile::replaceRoad(const features::Road& old_road, features::Road& new_road)
{
   for (auto i(roads_.begin()), end(roads_.end()); i!= end; ++i)
   {
      std::shared_ptr<features::Feature>& ptr = *i;

      if (&old_road == ptr.get())
         ptr = new_road.shared_from_this();
   }

   for (int i = 0; i < 4; ++i)
   {
      TileEdge& edge = edges_[i];
      if (edge.type == TileEdge::TYPE_ROAD && edge.road == &old_road)
         edge.road = &new_road;
   }
}
   
} // namespace carcassonne
