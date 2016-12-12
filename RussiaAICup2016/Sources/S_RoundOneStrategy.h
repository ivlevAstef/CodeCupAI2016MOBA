//
//File: S_RoundOneStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 11/12/2016
//

#pragma once

#include "S_BaseStrategyComponents.h"

namespace AICup
{
  class RoundOneStrategy: public BaseStrategyComponents {
  public:
    RoundOneStrategy(const CommandFabric& fabric);

    void update(const model::Wizard& self, model::Move& move) override;
  };
};