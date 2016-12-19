//
//File: CM_CommandMoveToBonus.h
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#pragma once

#include "CM_MoveCommand.h"

namespace AICup
{
  class CommandMoveToBonus: public MoveCommand {
  public:
    CommandMoveToBonus(Algorithm::PathFinder& finder);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    Algorithm::PathFinder& pathFinder;

    Position bonusPos;
  };
}