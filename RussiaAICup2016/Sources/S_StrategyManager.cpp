//
//File: CM_CommandManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "S_StrategyManager.h"

#ifdef ENABLE_TESTS
#include "S_TestMoveStrategy.h"
#include "S_TestMoveAndAttackStrategy.h"
#include "S_TestFollowStrategy.h"
#endif
#include "S_FirstStrategy.h"

using namespace AICup;

StrategyManager::StrategyManager(): fabric(pathFinder) {
  currentStrategy = std::make_shared<FirstStrategy>(fabric, pathFinder);
}

void StrategyManager::update(const model::Wizard& self, model::Move& move) {
  pathFinder.calculate(self);

  if (nullptr != currentStrategy.get()) {
    currentStrategy->update(self, move);
  }
}

#ifdef ENABLE_VISUALIZATOR
void StrategyManager::visualization(const Visualizator& visualizator) const {
  if (nullptr != currentStrategy.get()) {
    currentStrategy->visualization(visualizator);
  }

  pathFinder.visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR