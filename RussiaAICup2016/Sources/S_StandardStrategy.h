//
//File: S_StandardStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 11/12/2016
//

#pragma once

#include "S_BaseStrategyComponents.h"
#include "R_TacticsRole.h"

#include <unordered_map>

namespace AICup
{
  class StandardStrategy: public BaseStrategyComponents {
  public:
    StandardStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder);

    void update(const model::Wizard& self, model::Move& move) override;

  private:
    void initializeRoles();

    /// для командрой игры, чтобы быстро понимать кто на какой роли
    std::unordered_map<long long, TacticsRole> roles;
  };
};