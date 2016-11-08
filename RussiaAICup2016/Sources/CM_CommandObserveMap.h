//
//File: CM_CommandObserveMap.h
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#pragma once

#include "CM_Command.h"

namespace AICup
{
  class CommandObserveMap: public Command {
  public:
    CommandObserveMap();

    bool execute();

    const std::vector<CommandPtr>& neededCommands(const CommandFabric& fabric);

  private:
    std::vector<CommandPtr> needCommands;
  };
}