//
//File: S_AntiRushAttackStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 12/12/2016
//

#pragma once

#include "S_BaseStrategyComponents.h"

namespace AICup
{
  class AntiRushStrategy: public BaseStrategyComponents {
  public:
    AntiRushStrategy(const CommandFabric& fabric);

    void update(const model::Wizard& self, model::Move& move) override;

  private:
    void addMoveToBottom(const Wizard& self);
  };
};