#include "CM_MovePriorities.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "C_Math.h"

using namespace AICup;

int MovePriorities::avoidEnemy(const model::Wizard& self, const model::CircularUnit& enemy) {
  const auto constants = Game::instance().model();

  const auto selfPos = EX::pos(self);
  const auto enemyPos = EX::pos(enemy);
  const double distance = (selfPos - enemyPos).length();

  if (EX::isProjectile(enemy)) {
    return 1500;
  }

  const model::LivingUnit* checkLivingUnit = dynamic_cast<const model::LivingUnit*>(&enemy);
  assert(nullptr != checkLivingUnit);
  if (nullptr == checkLivingUnit) {
    return 0;
  }

  const model::LivingUnit& livingUnit = *checkLivingUnit;

  int lifePriority = (200 * livingUnit.getLife()) / livingUnit.getMaxLife();

  /// если врага можно быстро добить, то боятся его стоит меньше
  if (livingUnit.getLife() < EX::magicMissleAttack(self) * 2) {
    lifePriority = -500;
  }

  if (EX::isWizard(livingUnit)) {
    const int veryNearPrior = (distance < constants.getStaffRange() + self.getRadius()) ? 400 : 0;
    return 300 + lifePriority - EX::minTimeForMagic(EX::asWizard(livingUnit)) * 5 + veryNearPrior;
  } else if (EX::isMinion(livingUnit)) {
    if (distance < constants.getOrcWoodcutterAttackRange() + self.getRadius() * 2) {
      return 800 + lifePriority;
    }
    return  150 + lifePriority;
  } else if (EX::isBuilding(livingUnit)) {
    return 800 + lifePriority;
  }

  return 0;
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