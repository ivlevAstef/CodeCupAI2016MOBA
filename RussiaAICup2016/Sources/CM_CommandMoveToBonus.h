//
//File: CM_CommandMoveToBonus.h
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#pragma once

#include "CM_MoveCommand.h"

namespace AICup
{
  class CommandMoveToBonus: public MoveCommand {
  public:
    CommandMoveToBonus();

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR


  private:
    double potensialExpirience(const model::Wizard& self);

  private:
    MoveCommandPtr moveToBonus;
  };
}