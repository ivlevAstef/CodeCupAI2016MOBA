//
//File: E_Game.cpp
//Author: Ivlev Alexander. Stef
//Created: 09/11/2016
//

#include "E_Game.h"

using namespace AICup;

void Game::update(const model::Game& game, const model::Wizard& self) {
  if (!isInitialized) {
    srand(game.getRandomSeed());
    isInitialized = true;
  }

  modelGame = &game;
  setFaction(self);
}


model::Faction Game::friendFaction() {
  return instance().friendFactionValue;
}

model::Faction Game::enemyFaction() {
  return instance().enemyFactionValue;
}

model::Faction Game::reverseFaction(model::Faction faction) {
  if (model::FACTION_ACADEMY == faction) {
    return model::FACTION_RENEGADES;
  } else if (model::FACTION_RENEGADES == faction) {
    return model::FACTION_ACADEMY;
  }
  return model::FACTION_NEUTRAL;

}

void Game::setFaction(const model::Wizard& self) {
  if (model::FACTION_ACADEMY == self.getFaction()) {
    friendFactionValue = model::FACTION_ACADEMY;
    enemyFactionValue = model::FACTION_RENEGADES;
  } else {
    friendFactionValue = model::FACTION_RENEGADES;
    enemyFactionValue = model::FACTION_ACADEMY;
  }
}