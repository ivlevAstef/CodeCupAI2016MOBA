#include "CM_MovePriorities.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "C_Math.h"

using namespace AICup;

int MovePriorities::avoidEnemy(const model::Wizard& self, const model::LivingUnit& enemy) {
  const auto constants = Game::instance().model();

  const auto selfPos = EX::pos(self);
  const auto enemyPos = EX::pos(enemy);
  const double distance = (selfPos - enemyPos).length();

  int lifePriority = (200 * enemy.getLife()) / enemy.getMaxLife();

  /// если врага можно быстро добить, то боятся его стоит меньше
  if (enemy.getLife() < EX::magicMissleAttack(self) * 2) {
    lifePriority = -500;
  }

  if (EX::isWizard(enemy)) {
    const int veryNearPrior = (distance < constants.getStaffRange() + self.getRadius()) ? 400 : 0;
    return MAX(1, 300 + lifePriority - EX::minTimeForMagic(EX::asWizard(enemy)) * 5 + veryNearPrior);
  } else if (EX::isMinion(enemy)) {
    if (distance < constants.getOrcWoodcutterAttackRange() + self.getRadius() * 2) {
      return MAX(1, 800 + lifePriority);
    }
    return MAX(1, 150 + lifePriority);
  } else if (EX::isBuilding(enemy)) {
    return MAX(1, 800 + lifePriority);
  }

  return 1;
}

int MovePriorities::defendPoint(const model::Wizard&, const Position&) {
  return 0;
}

int MovePriorities::follow(const model::Wizard&, const model::LivingUnit&) {
  return 400;
}

int MovePriorities::getExpirience(const model::Wizard&, const model::LivingUnit&) {
  return 200;
}

int MovePriorities::keepDistance(const model::Wizard&, const Position, const double, const double) {
  return 500;
}

int MovePriorities::moveToBonus(const model::Wizard&, const Position&) {
  return 800;
}

int MovePriorities::moveToLine(const model::Wizard&, const model::LaneType&) {
  return 20;
}

int MovePriorities::moveToPoint(const model::Wizard&, const Position&) {
  return 10;
}

int MovePriorities::observeMap(const model::Wizard&) {
  return 0;
}