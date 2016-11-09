//
//File: CM_CommandFollow.h
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#pragma once

#include "CM_Command.h"

namespace AICup
{
  class CommandFollow: public Command {
  public:
    CommandFollow(long long unitId);

    bool check(const model::Wizard& self, model::Move& move);

    void execute(const model::Wizard& self, model::Move& move);

  private:
  };
}