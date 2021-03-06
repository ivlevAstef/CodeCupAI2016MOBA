//
//File: S_TestMoveAndAttackStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 13/11/2016
//

#pragma once

#include "S_CommandStrategy.h"

namespace AICup
{
  class TestMoveAndAttackStrategy: public CommandStrategy {
  public:
    TestMoveAndAttackStrategy(const CommandFabric& fabric);

    void update(const model::Wizard& self, model::Move& move) override;

  private:
    size_t currentMoveCommandIndex;
    std::vector<MoveCommandPtr> allMoveCommands;
  };
};