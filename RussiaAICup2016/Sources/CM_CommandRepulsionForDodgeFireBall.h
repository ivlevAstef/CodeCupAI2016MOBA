//
//File: CM_CommandRepulsionForDodgeFireBall.h
//Author: Ivlev Alexander. Stef
//Created: 20/12/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "model\Wizard.h"

namespace AICup
{
  class CommandRepulsionForDodgeFireBall: public MoveCommand {
  public:
    CommandRepulsionForDodgeFireBall();

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR
  };
}