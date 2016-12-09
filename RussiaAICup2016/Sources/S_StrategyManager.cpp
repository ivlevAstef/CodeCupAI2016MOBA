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
#include "S_TestDodgeStrategy.h"
#endif
#include "S_BaseStrategy.h"

using namespace AICup;

StrategyManager::StrategyManager(): fabric(pathFinder) {
}

void StrategyManager::update(const StrategyType& strategyType, const Wizard& self, model::Move& move) {
  pathFinder.calculate(self);

  switch (strategyType) {
    case StrategyType::Base:
      strategy = std::make_shared<BaseStrategy>(fabric, pathFinder);
      break;

#ifdef ENABLE_TESTS
    case StrategyType::Test_Move:
      strategy = std::make_shared<TestMoveStrategy>(fabric, pathFinder);
      break;
    case StrategyType::Test_MoveAndAttack:
      strategy = std::make_shared<TestMoveAndAttackStrategy>(fabric, pathFinder);
      break;
    case StrategyType::Test_Follow:
      strategy = std::make_shared<TestFollowStrategy>(fabric, pathFinder);
      break;
    case StrategyType::Test_Dodge:
      strategy = std::make_shared<TestDodgeStrategy>(fabric, pathFinder);
      break;
#endif // ENABLE_TESTS
  }


  strategy->update(self, move);
}

#ifdef ENABLE_VISUALIZATOR
void StrategyManager::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (nullptr != strategy.get()) {
    strategy->visualization(self, visualizator);
  }

  pathFinder.visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR