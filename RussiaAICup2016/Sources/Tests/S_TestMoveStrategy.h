//
//File: S_TestMoveStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 13/11/2016
//

#pragma once

#include "S_CommandStrategy.h"

namespace AICup
{
  class TestMoveStrategy: public CommandStategy {
  public:
    TestMoveStrategy(const CommandFabric& fabric);

    void update(const model::Wizard& self, model::Move& move) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR


  private:
    size_t currentMoveCommandIndex;
    std::vector<CommandPtr> moveCommands;
  };
};