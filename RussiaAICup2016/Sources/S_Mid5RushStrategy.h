//
//File: S_Mid5RushStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 11/12/2016
//

#pragma once

#include "S_BaseStrategyComponents.h"

namespace AICup
{
  class Mid5RushStrategy: public BaseStrategyComponents {
  public:
    Mid5RushStrategy(const CommandFabric& fabric);

    void update(const model::Wizard& self, model::Move& move) override;
  };
};