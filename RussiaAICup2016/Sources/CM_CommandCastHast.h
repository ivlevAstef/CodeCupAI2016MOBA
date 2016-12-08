//
//File: CM_CommandCastHast.h
//Author: Ivlev Alexander. Stef
//Created: 30/11/2016
//


#pragma once

#include "CM_CastCommand.h"

namespace AICup
{
  class CommandCastHast: public CastCommand {
  public:
    CommandCastHast();

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;
  };
}