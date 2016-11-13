//
//File: CM_CommandMoveToPoint.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_Command.h"
#include "C_Vector2D.h"
#include "E_Types.h"
#include "CM_Move.h"

namespace AICup
{
  class CommandMoveToPoint: public Command {
  public:
    CommandMoveToPoint(const double x, const double y, const double speedLimit = SPEED_LIMIT_NOT_SET, const MoveStyle style = MOVE_WITH_ROTATE);

    bool check(const model::Wizard& self, model::Move& move);

    void execute(const model::Wizard& self, model::Move& move);

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const Position point;
    const double speedLimit;
    const MoveStyle style;

    Path path;
    ObstaclesGroups obstaclesGroups;

    double pathLength;
  };
}