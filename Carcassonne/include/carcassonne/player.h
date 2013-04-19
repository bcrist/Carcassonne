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
// File: carcassonne/player.h
//
// Represents a human or AI player, including name and win/loss record, and
// skill level (for AI players).  Additionally, the player class stores
// some information specific to the current scenario (game).

#ifndef CARCASSONNE_PLAYER_H_
#define CARCASSONNE_PLAYER_H_
#include "carcassonne/_carcassonne.h"

#include <vector>

#include "carcassonne/follower.h"

namespace carcassonne {

class Player
{
public:

   // I thought of using SKILL_LEVEL_YOUNGLING, _PADAWAN, _KNIGHT, and _MASTER
   // but it'd probably just lead to confusion later on.  Plus it doesn't
   // really fit the medieval theme...
   enum SkillLevel {
      SKILL_LEVEL_TRIVIAL,
      SKILL_LEVEL_EASY,
      SKILL_LEVEL_NORMAL,
      SKILL_LEVEL_HARD
   };

   Player(const std::string& name, bool human);

   const std::string& getName() const;
   bool isHuman() const;
   SkillLevel getSkillLevel() const;

   void recordScenarioResult(bool won);

   int getWins() const;
   int getLosses() const;
   int getGamesPlayed() const;
   float getWinLossRatio() const;

   int getHighScore() const;

   // Specific to current game:

   const glm::vec4& getColor() const;

   // get an idle follower, or null if there are none
   Follower* getIdleFollower();

   // increase score_;
   void scorePoints(int points);

   // display this player's HUD if it's their turn
   void draw() const;

private:
   std::string name_;
   bool human_;
   SkillLevel skill_level_;

   int wins_;
   int losses_;
   int games_played_;

   int high_score_;


   // Specific to current game:
   glm::vec4 color_;

   std::vector<Follower> followers_;
   int idle_followers_;

   int score_;

   // Disable copy-construction & assignment - do not implement
   Player(const Player&);
   void operator=(const Player&);
};

} // namespace carcassonne

#endif
