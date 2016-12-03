//
//File: E_Game.h
//Author: Ivlev Alexander. Stef
//Created: 09/11/2016
//

#pragma once

#include "model\Game.h"
#include "model\Faction.h"
#include "model\Wizard.h"
#include "C_Singleton.h"

namespace AICup
{
  class Game: public Singleton<Game> {
  public:
    Game() : isInitialized(false) {
      modelGame = nullptr;
      friendFactionValue = model::FACTION_NEUTRAL;
      enemyFactionValue = model::FACTION_NEUTRAL;
    }

    void update(const model::Game& game, const model::Wizard& self);

    static inline const model::Game& model() {
      return *instance().modelGame;
    }

    static model::Faction friendFaction();
    static model::Faction enemyFaction();

    static model::Faction reverseFaction(model::Faction faction);

  private:
    void setFaction(const model::Wizard& self);

  private:
    model::Faction friendFactionValue;
    model::Faction enemyFactionValue;

    const model::Game* modelGame;

    bool isInitialized;
  };
}