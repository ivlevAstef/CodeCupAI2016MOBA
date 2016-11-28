//
//File: CM_CommandAttackFireball.h
//Author: Ivlev Alexander. Stef
//Created: 29/11/2016
//


#pragma once

#include "CM_AttackCommand.h"
#include "model\Wizard.h"

namespace AICup
{
  /// �� ����������� ����� ������
  class CommandAttackFireball: public AttackCommand {
  public:
    CommandAttackFireball();

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

    double priority(const Wizard& self) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::LivingUnit* target;
  };
}