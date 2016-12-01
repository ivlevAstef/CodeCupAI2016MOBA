//
//File: CM_CommandAvoidWizard.h
//Author: Ivlev Alexander. Stef
//Created: 29/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "model\Wizard.h"

namespace AICup
{
  class CommandAvoidWizard: public MoveCommand {
  public:
    CommandAvoidWizard(const model::Wizard& wizard);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

    double priority(const Wizard& self) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::Wizard& wizard;

    double distance;
    double checkDistance;
  };
}