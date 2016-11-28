//
//File: CM_CommandAvoidMinion.h
//Author: Ivlev Alexander. Stef
//Created: 29/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "model\Minion.h"

namespace AICup
{
  class CommandAvoidMinion: public MoveCommand {
  public:
    CommandAvoidMinion(Algorithm::PathFinder& finder, const model::Minion& minion);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

    double priority(const Wizard& self) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::Minion& minion;

    MoveCommandPtr moveToPointCommand;
  };
}