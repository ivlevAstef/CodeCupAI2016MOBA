//
//File: CM_TestMoveAndAttackStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 13/11/2016
//

#pragma once

#include "CM_CommandStrategy.h"

namespace AICup
{
  class TestMoveAndAttackStrategy: public CommandStategy {
  public:
    TestMoveAndAttackStrategy(const CommandFabric& fabric);

    void update(const model::Wizard& self, model::Move& move) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR


  private:
    size_t currentMoveCommandIndex;
    std::vector<CommandPtr> moveCommands;

    CommandPtr attackCommand;
  };
};