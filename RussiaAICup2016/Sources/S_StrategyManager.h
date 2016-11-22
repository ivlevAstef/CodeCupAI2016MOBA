//
//File: S_StrategyManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "model\Wizard.h"
#include "model\Move.h"

#include "C_Singleton.h"

#include "CM_CommandFabric.h"
#include "S_CommandStrategy.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class StrategyManager: public Singleton<StrategyManager> {
  public:
    StrategyManager();

    void update(const model::Wizard& self, model::Move& move);

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR


  private:
    CommandStrategyPtr currentStrategy;
    CommandFabric fabric;
    Algorithm::PathFinder pathFinder;
  };
}