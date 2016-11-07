//
//File: CM_CommandDefendPointt.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_Command.h"

namespace AICup
{
  class CommandDefendPoint: public Command {
  public:
    CommandDefendPoint(double x, double y);

    bool execute();

    const std::vector<CommandPtr>& neededCommands(const CommandFabric& fabric);

  private:
    std::vector<CommandPtr> needCommands;
  };
}