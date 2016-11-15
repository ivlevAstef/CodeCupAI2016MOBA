//
//File: CM_CommandKeepDistance.h
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#pragma once

#include "CM_Command.h"
#include "CM_CommandMoveToPoint.h"

namespace AICup
{
  class CommandKeepDistance: public Command {
  public:
    CommandKeepDistance(const double x, const double y, const double minDistance, const double maxDistance);

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, model::Move& move) override;


#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const double x;
    const double y;
    const double minDistance;
    const double maxDistance;

    std::shared_ptr<CommandMoveToPoint> commandMoveToPoint;
  };
}