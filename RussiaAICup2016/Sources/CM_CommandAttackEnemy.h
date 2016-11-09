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

    bool check(const model::Wizard& self, model::Move& move);

    void execute(const model::Wizard& self, model::Move& move);

  private:
  };
}