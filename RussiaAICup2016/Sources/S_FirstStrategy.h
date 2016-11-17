//
//File: S_FisrtStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//

#pragma once

#include "S_CommandStrategy.h"

namespace AICup
{
  class FirstStrategy: public CommandStategy {
  public:
    FirstStrategy(const CommandFabric& fabric);

    void update(const model::Wizard& self, model::Move& move) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR


  private:
    void init(const model::Wizard& self, model::Move& move);

    CommandPtr createMaxPriorityAttackEnemy(const model::Wizard& self);
    void changeLane(const model::Wizard& self);


  private:
    bool isInitialized;
    model::LaneType myLine;
    int lastChangeLineTick;

    std::vector<CommandPtr> usedCommands;
    CommandPtr moveToBonus;
  };
};