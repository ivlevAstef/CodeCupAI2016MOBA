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

    bool check(const model::Wizard& self, model::Move& move);

    void execute(const model::Wizard& self, model::Move& move);

  private:
  };
}