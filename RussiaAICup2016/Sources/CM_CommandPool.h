//
//File: CM_CommandPool.h
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#pragma once

#include "CM_Command.h"

namespace AICup
{
  class CommandPool: public Command {
  public:
    CommandPool(long long neutralUnitId);

    bool execute(const model::Wizard& self, model::Move& move);

    const std::vector<CommandPtr>& neededCommands(const CommandFabric& fabric);

  private:
    std::vector<CommandPtr> needCommands;
  };
}