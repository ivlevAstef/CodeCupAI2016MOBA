//
//File: CM_CommandFollow.h
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#pragma once

#include "CM_MoveCommand.h"

namespace AICup
{
  class CommandFollow: public MoveCommand {
  public:
    CommandFollow(Algorithm::PathFinder& finder, const long long unitId, const double minDistance = 0, const double maxDistance = 10000);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;


#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const long long unitId;
    const double minDistance;
    const double maxDistance;

    const model::LivingUnit* unit;
    MoveCommandPtr commandMoveToPoint;
  };
}