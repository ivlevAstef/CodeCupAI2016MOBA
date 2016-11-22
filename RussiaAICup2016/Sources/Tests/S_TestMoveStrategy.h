//
//File: S_TestMoveStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 13/11/2016
//

#pragma once

#include "S_CommandStrategy.h"

namespace AICup
{
  class TestMoveStrategy: public CommandStrategy {
  public:
    TestMoveStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder);

    void update(const model::Wizard& self, model::Move& move) override;

  private:
    size_t currentMoveCommandIndex;
    std::vector<MoveCommandPtr> allMoveCommands;
  };
};