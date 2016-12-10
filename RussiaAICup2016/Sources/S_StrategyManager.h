//
//File: S_StrategyManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "E_Wizard.h"
#include "model\Move.h"
#include "S_StrategyType.h"
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

    void update(const StrategyDTO& strategyDTO, const Wizard& self, model::Move& move);

#ifdef ENABLE_VISUALIZATOR
    void visualization(const model::Wizard& self, const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR


  private:
    CommandStrategyPtr strategy;
    CommandFabric fabric;
    Algorithm::PathFinder pathFinder;

    StrategyDTO lastStrategyDTO;
  };
}