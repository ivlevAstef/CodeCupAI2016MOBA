//
//File: CM_CommandAvoidBuild.h
//Author: Ivlev Alexander. Stef
//Created: 29/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "model\Building.h"

namespace AICup
{
  class CommandAvoidBuild: public MoveCommand {
  public:
    CommandAvoidBuild(Algorithm::PathFinder& finder, const model::Building& build);

    bool check(const Wizard& self) override;

    void execute(const Wizard& self, Result& result) override;

    double priority(const Wizard& self) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const model::Building& build;

    MoveCommandPtr moveToPointCommand;
  };
}