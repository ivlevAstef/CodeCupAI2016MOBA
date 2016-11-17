//
//File: CM_CommandMoveToPoint.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "C_Vector2D.h"
#include "E_Types.h"
#include "A_Move.h"

namespace AICup
{
  class CommandMoveToPoint: public MoveCommand {
  public:
    CommandMoveToPoint(const double x, const double y, const TurnStyle style = TurnStyle::TURN);

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const Position point;
    const TurnStyle style;

    Path path;
    ObstaclesGroups obstaclesGroups;

    double pathLength;
  };
}