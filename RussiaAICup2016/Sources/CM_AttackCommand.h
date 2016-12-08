//
//File: CM_AttackCommand.h
//Author: Ivlev Alexander. Stef
//Created: 17/11/2016
//

#pragma once

#include "CM_Command.h"
#include "model\ActionType.h"

namespace AICup
{
  class AttackCommand: public Command {
  public:
    struct Result {
      const model::LivingUnit* unit;
      model::ActionType action;

      double priority;
    };

  public:
    /// исполнить команду
    virtual void execute(const Wizard& self, Result& result) = 0;
  };

  typedef std::shared_ptr<AttackCommand> AttackCommandPtr;
}

