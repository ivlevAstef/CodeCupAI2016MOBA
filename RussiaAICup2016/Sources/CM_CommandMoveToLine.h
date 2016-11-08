//
//File: CM_CommandMoveToLine.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_Command.h"
#include "model\LineType.h"

namespace AICup
{
  class CommandMoveToLine: public Command {
  public:
    CommandMoveToLine(model::LineType line);

    bool execute(const model::Wizard& self, model::Move& move);

    const std::vector<CommandPtr>& neededCommands(const CommandFabric& fabric);

  private:
    std::vector<CommandPtr> needCommands;
  };
}