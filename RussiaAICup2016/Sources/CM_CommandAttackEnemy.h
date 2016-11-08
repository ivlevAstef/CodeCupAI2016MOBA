//
//File: CM_CommandAttackEnemy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_Command.h"

namespace AICup
{
  class CommandAttackEnemy: public Command {
  public:
    CommandAttackEnemy(long long enemyId);

    bool execute(const model::Wizard& self, model::Move& move);

    const std::vector<CommandPtr>& neededCommands(const CommandFabric& fabric);

  private:
    std::vector<CommandPtr> needCommands;
  };
}