//
//File: CM_CommandMoveMeleeAttack.h
//Author: Ivlev Alexander. Stef
//Created: 10/12/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "model\Wizard.h"

namespace AICup
{
  /// Не сохраняемый между тиками
  class CommandMoveMeleeAttack: public MoveCommand {
  public:
    CommandMoveMeleeAttack(const model::LivingUnit& unit);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::LivingUnit& unit;

    double changeOfWin;
  };
}