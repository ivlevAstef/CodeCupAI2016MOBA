//
//File: CM_CastCommand.h
//Author: Ivlev Alexander. Stef
//Created: 17/11/2016
//

#pragma once

#include "CM_Command.h"
#include "model\ActionType.h"

namespace AICup
{
  class CastCommand: public Command {
  public:
    struct Result {
      const model::LivingUnit* unit;
      model::ActionType action;
    };

  public:
    /// ��������� �������
    virtual void execute(const Wizard& self, Result& result) = 0;
  };

  typedef std::shared_ptr<CastCommand> CastCommandPtr;
}

