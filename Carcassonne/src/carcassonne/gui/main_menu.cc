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
// File: carcassonne/gui/main_menu.cc

#include "carcassonne/gui/main_menu.h"

#include <sstream>

#include "carcassonne/game.h"
#include "carcassonne/asset_manager.h"

namespace carcassonne {
namespace gui {

MainMenu::MainMenu(Game& game)
   : Menu(game)
{
   for (int i = 0; i < 6; ++i)
      player_types[i] = NONE;

   player_types[0] = HUMAN;
   player_types[1] = HUMAN;

   AssetManager& assets = game.getAssetManager();

   buttons_.push_back(Button());

   buttons_.back().setDrawingPlane(gfx::Rect(0.05f, 0.5f, 0.4f, 0.1f));
   buttons_.back().setEventPlane(gfx::Rect(0.05f, 0.5f, 0.4f, 0.1f));
   buttons_.back().setFont(assets.getTextureFont("kingthings"));
   buttons_.back().setTextYOffset(0.07f);
   buttons_.back().setTextColor(Button::STATE_NORMAL, glm::vec4(0.9f,0.9f,0.9f,1));
   buttons_.back().setTextColor(Button::STATE_HOVERED, glm::vec4(1.f,1.f,1.f,1));
   buttons_.back().setTextColor(Button::STATE_ACTIVE, glm::vec4(0.7f,0.7f,0.7f,1));
   buttons_.back().setTextScale(0.66f);
   buttons_.back().setText("P1: Human");
   buttons_.back().setAction([=](){ nextPlayerType(0); });

   buttons_.push_back(Button());
   buttons_.back().setDrawingPlane(gfx::Rect(0.05f, 0.65f, 0.4f, 0.1f));
   buttons_.back().setEventPlane(gfx::Rect(0.05f, 0.65f, 0.4f, 0.1f));
   buttons_.back().setFont(assets.getTextureFont("kingthings"));
   buttons_.back().setTextYOffset(0.07f);
   buttons_.back().setTextColor(Button::STATE_NORMAL, glm::vec4(0.9f,0.9f,0.9f,1));
   buttons_.back().setTextColor(Button::STATE_HOVERED, glm::vec4(1.f,1.f,1.f,1));
   buttons_.back().setTextColor(Button::STATE_ACTIVE, glm::vec4(0.7f,0.7f,0.7f,1));
   buttons_.back().setTextScale(0.66f);
   buttons_.back().setText("P2: Human");
   buttons_.back().setAction([=](){ nextPlayerType(1); });

   buttons_.push_back(Button());
   buttons_.back().setDrawingPlane(gfx::Rect(0.05f, 0.8f, 0.4f, 0.1f));
   buttons_.back().setEventPlane(gfx::Rect(0.05f, 0.8f, 0.4f, 0.1f));
   buttons_.back().setFont(assets.getTextureFont("kingthings"));
   buttons_.back().setTextYOffset(0.07f);
   buttons_.back().setTextColor(Button::STATE_NORMAL, glm::vec4(0.9f,0.9f,0.9f,1));
   buttons_.back().setTextColor(Button::STATE_HOVERED, glm::vec4(1.f,1.f,1.f,1));
   buttons_.back().setTextColor(Button::STATE_ACTIVE, glm::vec4(0.7f,0.7f,0.7f,1));
   buttons_.back().setTextScale(0.66f);
   buttons_.back().setText("P3: None");
   buttons_.back().setAction([=](){ nextPlayerType(2); });


   buttons_.push_back(Button());
   buttons_.back().setDrawingPlane(gfx::Rect(0.55f, 0.5f, 0.4f, 0.1f));
   buttons_.back().setEventPlane(gfx::Rect(0.55f, 0.5f, 0.4f, 0.1f));
   buttons_.back().setFont(assets.getTextureFont("kingthings"));
   buttons_.back().setTextYOffset(0.07f);
   buttons_.back().setTextColor(Button::STATE_NORMAL, glm::vec4(0.9f,0.9f,0.9f,1));
   buttons_.back().setTextColor(Button::STATE_HOVERED, glm::vec4(1.f,1.f,1.f,1));
   buttons_.back().setTextColor(Button::STATE_ACTIVE, glm::vec4(0.7f,0.7f,0.7f,1));
   buttons_.back().setTextScale(0.66f);
   buttons_.back().setText("P4: None");
   buttons_.back().setAction([=](){ nextPlayerType(3); });

   buttons_.push_back(Button());
   buttons_.back().setDrawingPlane(gfx::Rect(0.55f, 0.65f, 0.4f, 0.1f));
   buttons_.back().setEventPlane(gfx::Rect(0.55f, 0.65f, 0.4f, 0.1f));
   buttons_.back().setFont(assets.getTextureFont("kingthings"));
   buttons_.back().setTextYOffset(0.07f);
   buttons_.back().setTextColor(Button::STATE_NORMAL, glm::vec4(0.9f,0.9f,0.9f,1));
   buttons_.back().setTextColor(Button::STATE_HOVERED, glm::vec4(1.f,1.f,1.f,1));
   buttons_.back().setTextColor(Button::STATE_ACTIVE, glm::vec4(0.7f,0.7f,0.7f,1));
   buttons_.back().setTextScale(0.66f);
   buttons_.back().setText("P5: None");
   buttons_.back().setAction([=](){ nextPlayerType(4); });

   buttons_.push_back(Button());
   buttons_.back().setDrawingPlane(gfx::Rect(0.55f, 0.8f, 0.4f, 0.1f));
   buttons_.back().setEventPlane(gfx::Rect(0.55f, 0.8f, 0.4f, 0.1f));
   buttons_.back().setFont(assets.getTextureFont("kingthings"));
   buttons_.back().setTextYOffset(0.07f);
   buttons_.back().setTextColor(Button::STATE_NORMAL, glm::vec4(0.9f,0.9f,0.9f,1));
   buttons_.back().setTextColor(Button::STATE_HOVERED, glm::vec4(1.f,1.f,1.f,1));
   buttons_.back().setTextColor(Button::STATE_ACTIVE, glm::vec4(0.7f,0.7f,0.7f,1));
   buttons_.back().setTextScale(0.66f);
   buttons_.back().setText("P6: None");
   buttons_.back().setAction([=](){ nextPlayerType(5); });


   buttons_.push_back(Button());
   buttons_.back().setDrawingPlane(gfx::Rect(0.05f, 0.05f, 0.9f, 0.2f));
   buttons_.back().setFont(assets.getTextureFont("kingthings"));
   buttons_.back().setTextYOffset(0.12f);
   buttons_.back().setTextColor(Button::STATE_NORMAL, glm::vec4(0.9f,0.9f,0.9f,1));
   buttons_.back().setTextScale(1.5f);
   buttons_.back().setText("Carcassonne!");

   buttons_.push_back(Button());
   buttons_.back().setDrawingPlane(gfx::Rect(0.225f, 0.3f, 0.55f, 0.15f));
   buttons_.back().setEventPlane(gfx::Rect(0.225f, 0.3f, 0.55f, 0.15f));
   buttons_.back().setFont(assets.getTextureFont("kingthings"));
   buttons_.back().setTextYOffset(0.12f);
   buttons_.back().setTextColor(Button::STATE_NORMAL, glm::vec4(0.9f,0.9f,0.9f,1));
   buttons_.back().setTextColor(Button::STATE_HOVERED, glm::vec4(1.f,1.f,1.f,1));
   buttons_.back().setTextColor(Button::STATE_ACTIVE, glm::vec4(0.7f,0.7f,0.7f,1));
   buttons_.back().setText("New Game");
   buttons_.back().setAction([=](){ newGame(); });
   

}

MainMenu::MainMenu(const MainMenu& other)
   : Menu(other)
{
   for (int i = 0; i < 6; ++i)
      player_types[i] = other.player_types[i];

   buttons_[0].setAction([=](){ nextPlayerType(0); });
   buttons_[1].setAction([=](){ nextPlayerType(1); });
   buttons_[2].setAction([=](){ nextPlayerType(2); });
   buttons_[3].setAction([=](){ nextPlayerType(3); });
   buttons_[4].setAction([=](){ nextPlayerType(4); });
   buttons_[5].setAction([=](){ nextPlayerType(5); });

   buttons_[7].setAction([=](){ newGame(); });
}

MainMenu::~MainMenu()
{
}

std::unique_ptr<Menu> MainMenu::clone() const
{
   return std::unique_ptr<Menu>(new MainMenu(*this));
}

void MainMenu::draw()
{
   gfx::Texture::disableAny();
   glColor4f(0.15, 0.35, 0.55, 1);
   glRectf(-10, 10, 10, -10);

   Menu::draw();
}

void MainMenu::nextPlayerType(int player)
{
   std::ostringstream oss;
   oss << "P" << (player + 1) << ": ";

   switch (player_types[player])
   {
      case HUMAN:
         player_types[player] = MACHINE;
         oss << "Machine";
         break;

      case MACHINE:
         player_types[player] = NONE;
         oss << "None";
         break;

      default:
         player_types[player] = HUMAN;
         oss << "Human";
         break;
   }
   buttons_[player].setText(oss.str());
}

void MainMenu::newGame()
{
   ScenarioInit sci;

   for (int i = 0; i < 6; ++i)
   {
      if (player_types[i] != NONE)
      {
         sci.players.push_back(game_.getPlayer(i));
         sci.players.back()->setHuman(player_types[i] == HUMAN);
         sci.players.back()->newScenario(game_.getAssetManager(), sci.players.back()->getColor());
      }
   }

   Pile p = game_.getAssetManager().getTileSet("std-base");
   sci.starting_tile = p.remove();
   p.shuffle();
   sci.tiles = std::move(p);
   game_.newScenario(sci);
   game_.unifier.schedule([=](sf::Time t) -> bool { game_.clearMenus(); return true; });
}


} // namespace carcassonne::gui
} // namespace carcassonne

