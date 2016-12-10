//
//File: S_BaseStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//

#pragma once

#include "S_CommandStrategy.h"

namespace AICup
{
  class BaseStrategy: public CommandStrategy {
  public:
    BaseStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder);

    void update(const Wizard& self, model::Move& move) override;

  private:
    void init(const Wizard& self);

    model::LaneType checkAndChangeLane(const Wizard& self);

    void addAroundEnemiesOrMoveMelee(const Wizard& self);
    void addAttackFollow(const Wizard& self);

    void addMoveTo(const Wizard& self, model::LaneType lane);

    void addAttacks(const Wizard& self);
    void addCasts(const Wizard& self);
  };
};