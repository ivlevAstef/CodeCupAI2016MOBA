//
//File: E_Game.h
//Author: Ivlev Alexander. Stef
//Created: 09/11/2016
//

#pragma once

#include "model\Game.h"
#include "model\Faction.h"
#include "C_Singleton.h"

namespace AICup
{
  class Game: public Singleton<Game> {
  public:
    void update(const model::Game& game);

    static inline const model::Game& model() {
      return *instance().modelGame;
    }

    model::Faction enemyFaction(const model::Faction faction) const;

  private:
    const model::Game* modelGame;
  };
}