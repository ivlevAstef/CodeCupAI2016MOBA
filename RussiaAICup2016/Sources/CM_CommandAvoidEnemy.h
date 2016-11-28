//
//File: CM_CommandAvoidEnemy.h
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "model\Minion.h"
#include "model\Building.h"

namespace AICup
{
  class CommandAvoidEnemy: public MoveCommand {
  public:
    CommandAvoidEnemy(Algorithm::PathFinder& finder, const long long enemyId);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

    friend class CommandAvoidAround;

  private:
    const long long enemyId;
    MoveCommandPtr moveToPointCommand;

    const model::CircularUnit* enemy;
  };
}