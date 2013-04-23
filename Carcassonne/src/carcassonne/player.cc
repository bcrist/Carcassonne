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
// File: carcassonne/player.cc
//
// Represents a human or AI player, including name and win/loss record, and
// skill level (for AI players).  Additionally, the player class stores
// some information specific to the current scenario (game).

#include "carcassonne/player.h"

#include <sstream>

#include "carcassonne/asset_manager.h"

namespace carcassonne {

Player::Player(const std::string& name, bool human)
   : name_(name),
     human_(human),
     skill_level_(SKILL_LEVEL_TRIVIAL),
     wins_(0),
     losses_(0),
     games_played_(0),
     high_score_(0)
{
}

const std::string& Player::getName() const
{
   return name_;
}

void Player::setHuman(bool human)
{
   human_ = human;
}

bool Player::isHuman() const
{
   return human_;
}

Player::SkillLevel Player::getSkillLevel() const
{
   return Player::SKILL_LEVEL_TRIVIAL;
}

void Player::recordScenarioResult(bool won)
{
   games_played_++;
   if(won)
      wins_++;
   else
      losses_++;
   if(high_score_ < score_)
      high_score_ = score_;
}

int Player::getWins() const
{
   return wins_;
}

int Player::getLosses() const
{
   return losses_;
}

int Player::getGamesPlayed() const
{
   return games_played_;
}

float Player::getWinLossRatio() const
{
   return wins_ / float(games_played_);
}

int Player::getHighScore() const
{
   return high_score_;
}

// Specific to current game:

void Player::newScenario(AssetManager& asset_mgr, const glm::vec4& color)
{
   

   color_ = color;
   followers_.clear();
   for (int i = 0; i < 7; ++i)
   {
      followers_.push_back(Follower(asset_mgr, *this));
      followers_.back().setIdle(true);
   }

   score_ = 0;
}

void Player::setColor(const glm::vec4& color)
{
   color_ = color;
}

const glm::vec4& Player::getColor() const
{
   return color_;
}

// get an idle follower, or null if there are none
Follower* Player::getIdleFollower()
{
   for(auto i(followers_.rbegin()), end(followers_.rend()); i != end ; ++i)
   {
      if (i->isIdle())
         return &(*i);
   }
   
   return nullptr;
}

size_t Player::getIdleFollowerCount() const
{
   size_t count = 0;
   for(auto i(followers_.begin()), end(followers_.end()); i != end ; ++i)
   {
      if (i->isIdle())
         ++count;
   }

   return count;
}

void Player::organizeIdleFollowers()
{
   size_t count = 0;
   for(auto i(followers_.begin()), end(followers_.end()); i != end ; ++i)
   {
      if (i->isIdle())
      {
         ++count;

         i->setPosition(glm::vec3(0.5f * count, 0, 0));
      }
   }
}

// increase score_;
void Player::scorePoints(int points)
{
   score_ += points;

   std::cout << getName() << " scored " << points << " points!  Score: " << score_ << std::endl;
}

int Player::getScore() const
{
   return score_;
}

// display this player's HUD if it's their turn
void Player::draw() const
{
   glPushMatrix();
   glTranslatef(0.0f, 0.95f, 0.0f);
   glScalef(0.15f, -0.15f, 0.15f);

   for (auto i(followers_.begin()), end(followers_.end()); i != end; ++i)
      if (i->isIdle() && !i->isFloating())
         i->draw();

   glPopMatrix();
}

void Player::drawPlacedFollowers() const
{
   for (auto i(followers_.begin()), end(followers_.end()); i != end; ++i)
   {
      if (i->isPlaced())
         i->draw();
   }
}

} // namespace carcassonne
