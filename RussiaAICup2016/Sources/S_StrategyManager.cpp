//
//File: CM_CommandManager.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "S_StrategyManager.h"
#include "E_Wizard.h"

#ifdef ENABLE_TESTS
#include "S_TestMoveStrategy.h"
#include "S_TestMoveAndAttackStrategy.h"
#include "S_TestFollowStrategy.h"
#include "S_TestDodgeStrategy.h"
#endif

#include "S_RoundOneStrategy.h"
#include "S_RoundTwoStrategy.h"
#include "S_StandardStrategy.h"
#include "S_Mid5RushStrategy.h"
#include "S_AntiRushStrategy.h"

using namespace AICup;

StrategyManager::StrategyManager() {
  lastStrategyDTO = {StrategyType::RoundTwo, {model::_LANE_UNKNOWN_, 0, 0}};
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

void StrategyManager::update(const StrategyDTO& strategyDTO, const model::Wizard& self, model::Move& move) {
  if (lastStrategyDTO != strategyDTO || nullptr == strategy) {
    switch (strategyDTO.type) {
      case StrategyType::RoundOne:
        strategy = std::make_shared<RoundOneStrategy>(fabric);
        break;

      case StrategyType::RoundTwo:
        strategy = std::make_shared<RoundTwoStrategy>(fabric);
        break;

      /// коммандные
      case StrategyType::Standart:
        strategy = std::make_shared<StandardStrategy>(fabric);
        break;
      case StrategyType::AntiRush:
        strategy = std::make_shared<AntiRushStrategy>(fabric);
        break;
      case StrategyType::Rush5:
        strategy = std::make_shared<Mid5RushStrategy>(fabric);
        break;
      case StrategyType::Observer:
        exit(1);
      case StrategyType::KillTop:
        exit(1);
      case StrategyType::Push:
        exit(1);
      case StrategyType::Win:
        exit(1);
      case StrategyType::Attack:
        exit(1);
      case StrategyType::Defense:
        exit(1);

#ifdef ENABLE_TESTS
      case StrategyType::Test_Move:
        strategy = std::make_shared<TestMoveStrategy>(fabric);
        break;
      case StrategyType::Test_MoveAndAttack:
        strategy = std::make_shared<TestMoveAndAttackStrategy>(fabric);
        break;
      case StrategyType::Test_Follow:
        strategy = std::make_shared<TestFollowStrategy>(fabric);
        break;
      case StrategyType::Test_Dodge:
        strategy = std::make_shared<TestDodgeStrategy>(fabric);
        break;
#endif // ENABLE_TESTS
    }

    lastStrategyDTO = strategyDTO;
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