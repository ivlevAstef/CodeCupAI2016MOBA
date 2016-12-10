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
  lastStrategyDTO = {StrategyType::Base, {model::_LANE_UNKNOWN_, 0, 0}};
}

bool operator==(const StrategyDTO& dto1, const StrategyDTO& dto2) {
  if (dto1.type != dto2.type) {
    return false;
  }

  const auto& d1 = dto1.data;
  const auto& d2 = dto2.data;

  switch (dto1.type) {
    case StrategyType::Standart:
      return d1.lane == d2.lane;
    case StrategyType::AntiRush:
      return d1.lane == d2.lane && d1.attactTick == d2.attactTick;
    case StrategyType::Push:
      return d1.lane == d2.lane;
    case StrategyType::Attack:
      return d1.attackedWizardId == d2.attackedWizardId && d1.attactTick == d2.attactTick;
    case StrategyType::Defense:
      return d1.lane == d2.lane;
  }

  return true;
}
bool operator!=(const StrategyDTO& dto1, const StrategyDTO& dto2) {
  return !operator==(dto1, dto2);
}

void StrategyManager::update(const StrategyDTO& strategyDTO, const Wizard& self, model::Move& move) {
  pathFinder.calculate(self);

  if (lastStrategyDTO != strategyDTO || nullptr == strategy) {
    switch (strategyDTO.type) {
      case StrategyType::Base:
        strategy = std::make_shared<BaseStrategy>(fabric, pathFinder);
        break;

      /// коммандные
      case StrategyType::Standart:
        strategy = std::make_shared<BaseStrategy>(fabric, pathFinder);
        break;
      case StrategyType::AntiRush:
        strategy = std::make_shared<BaseStrategy>(fabric, pathFinder);
        break;
      case StrategyType::Observer:
        strategy = std::make_shared<BaseStrategy>(fabric, pathFinder);
        break;
      case StrategyType::KillTop:
        strategy = std::make_shared<BaseStrategy>(fabric, pathFinder);
        break;
      case StrategyType::Push:
        strategy = std::make_shared<BaseStrategy>(fabric, pathFinder);
        break;
      case StrategyType::Win:
        strategy = std::make_shared<BaseStrategy>(fabric, pathFinder);
        break;
      case StrategyType::Attack:
        strategy = std::make_shared<BaseStrategy>(fabric, pathFinder);
        break;
      case StrategyType::Defense:
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