//
//File: S_TestDodgeStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 05/12/2016
//

#pragma once

#include "S_CommandStrategy.h"

namespace AICup
{
  class TestDodgeStrategy: public CommandStrategy {
  public:
    TestDodgeStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder);

    void update(const Wizard& self, model::Move& move) override;
  };
};