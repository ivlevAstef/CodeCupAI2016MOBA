//
//File: CM_CommandAvoidEnemy.h
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//


#pragma once

#include "CM_Command.h"
#include "model\Minion.h"
#include "model\Building.h"

namespace AICup
{
  class CommandAvoidEnemy: public Command {
  public:
    CommandAvoidEnemy(const long long enemyId);

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, model::Move& move) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

    friend class CommandAvoidAround;

  private:
    const long long enemyId;
    CommandPtr followCommand;
    double distance;

    const model::LivingUnit* enemy;
    const model::Wizard* wizardEnemy;
    const model::Minion* minionEnemy;
    const model::Building* buildEnemy;
  };
}