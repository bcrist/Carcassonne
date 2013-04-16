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
// File: carcassonne/player.cc
//
// Represents a human or AI player, including name and win/loss record, and
// skill level (for AI players).  Additionally, the player class stores
// some information specific to the current scenario (game).

#include "carcassonne/player.h"

namespace carcassonne {

Player::Player(const std::string& name, bool human)
{
}

const std::string& Player::getName() const
{
   return name_;
}

bool Player::isHuman() const
{
   return false;
}

Player::SkillLevel Player::getSkillLevel() const
{
   return Player::SKILL_LEVEL_TRIVIAL;
}

void Player::recordScenarioResult(bool won)
{
}

int Player::getWins() const
{
   return 0;
}

int Player::getLosses() const
{
   return 0;
}

int Player::getGamesPlayed() const
{
   return 0;
}

float Player::getWinLossRatio() const
{
   return 0;
}

int Player::getHighScore() const
{
   return 0;
}

// Specific to current game:

// get an idle follower, or null if there are none
Follower* Player::getIdleFollower()
{
   return nullptr;
}

// increase score_;
void Player::scorePoints(int points)
{
}

// display this player's HUD if it's their turn
void Player::draw() const
{
}

} // namespace carcassonne
