//
//File: CM_CommandAttackTree.h
//Author: Ivlev Alexander. Stef
//Created: 28/11/2016
//

#pragma once

#include "CM_AttackCommand.h"
#include "model\Tree.h"

namespace AICup
{
  /// Не сохраняемый между тиками
  class CommandAttackTree: public AttackCommand {
  public:
    CommandAttackTree(const model::Tree& tree);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

    double priority(const Wizard& self) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::Tree& tree;
  };
}