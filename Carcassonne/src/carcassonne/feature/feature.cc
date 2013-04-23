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
// File: carcassonne/features/feature.h
//
// Base class for cities, cloisters, farms, and roads.

#include "carcassonne\features\feature.h"

#include "carcassonne\tile.h"

namespace carcassonne {
namespace features {

Feature::Feature()
{
}

// copies an existing feature, without adding tiles or followers
Feature::Feature(const Feature& other)
{
   if (other.follower_placeholder_)
      follower_placeholder_.reset(new Follower(*other.follower_placeholder_));
}

Feature::~Feature()
{
}
//returns true if getType is a TYPE_CITY
bool Feature::isCity() const
{
   return getType() == TYPE_CITY;
}
//returns true if getType is a TYPE_CLOISTER
bool Feature::isCloister() const
{
   return getType() == TYPE_CLOISTER;
}
// returns true if this feature is complete, and can be scored immediately.
// After each turn, this is called on each feature of the placed tile, and
// if it returns true, the feature's score() function is called.
bool Feature::isFarm() const
{
   return getType() == TYPE_FARM;
}
//returns true if getType is a TYPE_ROAD
bool Feature::isRoad() const
{
   return getType() == TYPE_ROAD;
}
//if true places placeholders in the correct position relative to the 
//tile displaying where a follower may be placed
bool Feature::hasPlaceholder() const
{
   return follower_placeholder_;
}

const Follower* Feature::getPlaceholder() const
{
   return follower_placeholder_.get();
}

void Feature::drawPlaceholder() const
{
   if (follower_placeholder_)
      follower_placeholder_->draw();
}

void Feature::setPlaceholderColor(const glm::vec4& color)
{
   if (follower_placeholder_)
      follower_placeholder_->setColor(color);
}

void Feature::placeFollower(Follower& follower, const Tile& relative_to)
{
   if (!follower_placeholder_)
      return;

   follower.setIdle(false);
   follower.setFloating(false);
   follower.setOrientation(*follower_placeholder_, relative_to);
   followers_.push_back(&follower);
   follower_placeholder_.reset();
}

}
}