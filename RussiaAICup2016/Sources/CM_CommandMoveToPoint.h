//
//File: CM_CommandMoveToPoint.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_Command.h"
#include "C_Vector2D.h"

namespace AICup
{
  class CommandMoveToPoint: public Command {
  public:
    CommandMoveToPoint(double x, double y);

    bool execute(const model::Wizard& self, model::Move& move);

    const std::vector<CommandPtr>& neededCommands(const CommandFabric& fabric);

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    Position point;
    std::vector<Position> path;
    double pathLength;
    std::vector<CommandPtr> needCommands;
  };
}