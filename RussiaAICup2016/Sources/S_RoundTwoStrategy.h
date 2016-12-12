//
//File: S_RoundTwoStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 11/12/2016
//

#pragma once

#include "S_BaseStrategyComponents.h"

namespace AICup
{
  class RoundTwoStrategy: public BaseStrategyComponents {
  public:
    RoundTwoStrategy(const CommandFabric& fabric);

    void update(const model::Wizard& self, model::Move& move) override;

  private:
    /// cмена скилл билда в зависимости от ситуации на линии
    void dynamicChangeSkillBuild(const model::Wizard& self);
  };
};