//
//File: CM_CommandDefendPoint.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_MoveCommand.h"

namespace AICup
{
  class CommandDefendPoint: public MoveCommand {
  public:
    CommandDefendPoint(Algorithm::PathFinder& finder, double x, double y);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

  private:
  };
}