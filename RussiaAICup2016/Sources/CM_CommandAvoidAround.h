//
//File: CM_CommandAvoidAround.h
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#pragma once

#include "CM_MoveCommand.h"
#include "C_Vector2D.h"

namespace AICup
{
  class CommandAvoidAround: public MoveCommand {
  public:
    CommandAvoidAround(Algorithm::PathFinder& finder);

    bool check(const model::Wizard& self) override;

    int priority(const model::Wizard& self) override;

    void execute(const model::Wizard& self, Result& result) override;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const override;
#endif // ENABLE_VISUALIZATOR


  private:
    struct EnemyData {
      Position pos;
      int priority;
      double minDistance;
    };

    std::vector<EnemyData> enemies;
  };
}