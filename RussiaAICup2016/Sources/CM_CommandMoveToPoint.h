//
//File: CM_CommandMoveToPoint.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_Command.h"
#include "C_Vector2D.h"
#include "E_Types.h"

namespace AICup
{
  class CommandMoveToPoint: public Command {
  public:
    CommandMoveToPoint(double x, double y);

    bool check(const model::Wizard& self, model::Move& move);

    void execute(const model::Wizard& self, model::Move& move);

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const Position point;

    Path path;
    ObstaclesGroups obstaclesGroups;

    double pathLength;
  };
}