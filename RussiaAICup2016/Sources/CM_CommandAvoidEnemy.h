//
//File: CM_CommandAvoidEnemy.h
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//


#pragma once

#include "CM_Command.h"

namespace AICup
{
  class CommandAvoidEnemy: public Command {
  public:
    CommandAvoidEnemy(const long long enemyId);

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, model::Move& move) override;

  private:
    const long long enemyId;
  };
}