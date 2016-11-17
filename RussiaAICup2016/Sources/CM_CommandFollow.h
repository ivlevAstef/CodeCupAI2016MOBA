//
//File: CM_CommandFollow.h
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#pragma once

#include "CM_MoveCommand.h"

namespace AICup
{
  class CommandFollow: public MoveCommand {
  public:
    CommandFollow(const long long unitId, const double minDistance = 0, const double maxDistance = 10000);

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, Result& result) override;


#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR

  private:
    const long long unitId;
    const double minDistance;
    const double maxDistance;

    MoveCommandPtr commandMoveToPoint;
  };
}