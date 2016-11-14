//
//File: CM_CommandAttackEnemy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "CM_Command.h"
#include "C_Vector2D.h"

namespace AICup
{
  class CommandAttackEnemy: public Command {
  public:
    CommandAttackEnemy(const long long enemyId);

    bool check(const model::Wizard& self, model::Move& move);

    void execute(const model::Wizard& self, model::Move& move);

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const long long enemyId;

    Position selfPos;
  };
}