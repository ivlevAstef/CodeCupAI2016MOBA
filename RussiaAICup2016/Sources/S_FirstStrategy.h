//
//File: S_FisrtStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//

#pragma once

#include "S_CommandStrategy.h"

namespace AICup
{
  class FirstStrategy: public CommandStrategy {
  public:
    FirstStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder);

    void update(const Wizard& self, model::Move& move) override;

  private:
    void init(const Wizard& self);

    void changeLane(const Wizard& self);

    const std::vector<MoveCommandPtr> calcAllAroundEnemies(const Wizard& self);

  private:
    bool isInitialized;
    model::LaneType myLine;
    int lastChangeLineTick;

    MoveCommandPtr moveToBonus;
  };
};