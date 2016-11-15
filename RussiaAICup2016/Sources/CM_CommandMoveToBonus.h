//
//File: CM_CommandMoveToBonus.h
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#pragma once

#include "CM_Command.h"

namespace AICup
{
  class CommandMoveToBonus: public Command {
  public:
    CommandMoveToBonus();

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, model::Move& move) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR


  private:
    double potensialExpirience(const model::Wizard& self);

  private:
    CommandPtr moveToBonus;
  };
}