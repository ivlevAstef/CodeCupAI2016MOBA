//
//File: CM_CommandSequence.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_Command.h"

namespace AICup
{
  class CommandSequence: public Command {
  public:
    CommandSequence(std::initializer_list<CommandPtr> commands);

    bool execute();

    const std::vector<CommandPtr>& neededCommands(const CommandFabric& fabric);

  private:
    std::vector<CommandPtr> needCommands;
  };
}