//
//File: CM_CommandObserveMap.h
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#pragma once

#include "CM_MoveCommand.h"

namespace AICup
{
  class CommandObserveMap: public MoveCommand {
  public:
    CommandObserveMap(Algorithm::PathFinder& finder);

    bool check(const model::Wizard& self) override;

    void execute(const model::Wizard& self, Result& result) override;

  private:
  };
}