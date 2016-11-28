//
//File: CM_CommandKeepDistance.h
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#pragma once

#include "CM_MoveCommand.h"

namespace AICup
{
  class CommandKeepDistance: public MoveCommand {
  public:
    CommandKeepDistance(Algorithm::PathFinder& finder, const double x, const double y, const double minDistance, const double maxDistance);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;


#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const double x;
    const double y;
    const double minDistance;
    const double maxDistance;

    MoveCommandPtr commandMoveToPoint;
  };
}