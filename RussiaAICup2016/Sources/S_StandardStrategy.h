//
//File: S_StandardStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 11/12/2016
//

#pragma once

#include "S_BaseStrategyComponents.h"
#include "R_TacticsRole.h"

namespace AICup
{
  class StandardStrategy: public BaseStrategyComponents {
  public:
    StandardStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder);

    void update(const model::Wizard& self, model::Move& move) override;

  private:
    void setupDefaultLane(const model::Wizard& self);

  private:
    model::LaneType defaultLane;
  };
};