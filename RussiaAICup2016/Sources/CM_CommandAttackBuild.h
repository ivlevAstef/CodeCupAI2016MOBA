//
//File: CM_CommandAttackBuild.h
//Author: Ivlev Alexander. Stef
//Created: 28/11/2016
//


#pragma once

#include "CM_AttackCommand.h"
#include "model\Building.h"

namespace AICup
{
  /// Не сохраняемый между тиками
  class CommandAttackBuild: public AttackCommand {
  public:
    CommandAttackBuild(const model::Building& build);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::Building& build;
  };
}