//
//File: CM_TestFollowStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 13/11/2016
//

#pragma once

#include "CM_CommandStrategy.h"

namespace AICup
{
  class TestFollowStrategy: public CommandStategy {
  public:
    TestFollowStrategy(const CommandFabric& fabric);

    void update(const model::Wizard& self, model::Move& move) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR


  private:
    CommandPtr moveCommand;
  };
};