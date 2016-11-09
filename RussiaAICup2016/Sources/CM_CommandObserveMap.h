//
//File: CM_CommandObserveMap.h
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#pragma once

#include "CM_Command.h"

namespace AICup
{
  class CommandObserveMap: public Command {
  public:
    CommandObserveMap();

    bool check(const model::Wizard& self, model::Move& move);

    void execute(const model::Wizard& self, model::Move& move);

  private:
  };
}