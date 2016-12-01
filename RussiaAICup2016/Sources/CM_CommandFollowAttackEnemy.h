//
//File: CM_CommandFollowAttack.h
//Author: Ivlev Alexander. Stef
//Created: 28/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "model\Wizard.h"

namespace AICup
{
  /// Не сохраняемый между тиками
  class CommandFollowAttack: public MoveCommand {
  public:
    CommandFollowAttack(const model::Wizard& wizard);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

    double priority(const Wizard& self) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::Wizard& wizard;
  };
}