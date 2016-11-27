//
//File: CM_CommandAttackEnemy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_AttackCommand.h"
#include "C_Vector2D.h"

namespace AICup
{
  class CommandAttackEnemy: public AttackCommand {
  public:
    CommandAttackEnemy(const long long enemyId);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const long long enemyId;

    Position selfPos;
  };
}