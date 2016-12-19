#include "T_Tactics.h"
#include "E_World.h"
#include "E_Game.h"
#include "A_PathFinder.h"

#include "S_AntiRushMath.h"
#include "C_Math.h"
#include "C_Extensions.h"

using namespace AICup;

std::vector<Message> Tactics::update(const model::Wizard& self, const std::unordered_map<long long, int>& idMessage) {
  std::vector<Message> result;

  /*if (useAntiRush(self, result)) {
    return finalResult(TacticsType::AntiRush, result);
  }*/

  if (useStandard(self, result)) {
    return finalResult(TacticsType::Standart, result);
  }

  return result;
}

bool Tactics::useAntiRush(const model::Wizard& self, std::vector<Message>& result) {
  /// если по миду мало магов, то это не rush ну или их уже убили)
  if (World::instance().wizardCount(model::LANE_MIDDLE, Game::enemyFaction()) < 4) {
    return false;
  }

  /// иначе это Rush и надо просчитать циферки

  result.clear();
  result.emplace_back(TacticsType::AntiRush);
  result.emplace_back(TacticsType::AntiRush);
  result.emplace_back(TacticsType::AntiRush);
  result.emplace_back(TacticsType::AntiRush);
  result.emplace_back(TacticsType::AntiRush);

  return true;
}


bool Tactics::useRush(const model::Wizard& self, std::vector<Message>& result) {
  return false;
  /// первых два тика шлем только информацию что хотим использовать раш
  if (World::model().getTickIndex() >= 1) {
    return false;
  }

  result.clear();
  result.emplace_back(TacticsType::Rush);
  result.emplace_back(TacticsType::Rush);
  result.emplace_back(TacticsType::Rush);
  result.emplace_back(TacticsType::Rush);
  result.emplace_back(TacticsType::Rush);

  return true;
}

bool Tactics::useStandard(const model::Wizard& self, std::vector<Message>& result) {
  result.clear();
  result.emplace_back(TacticsType::Standart);
  result.emplace_back(TacticsType::Standart);
  result.emplace_back(TacticsType::Standart);
  result.emplace_back(TacticsType::Standart);
  result.emplace_back(TacticsType::Standart);

  return true;
}

std::vector<Message> Tactics::finalResult(TacticsType tacticsType, const std::vector<Message>& data) {
  if (tacticsType == lastType) {
    return{ };
  }
  lastType = tacticsType;
  return data;
}