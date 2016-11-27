#include "CM_MovePriorities.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "C_Math.h"

using namespace AICup;

int MovePriorities::avoidEnemy(const Wizard& self, const model::CircularUnit& enemy) {
  const auto constants = Game::instance().model();

  const auto selfPos = EX::pos(self);
  const auto enemyPos = EX::pos(enemy);
  const double distance = (selfPos - enemyPos).length();

  if (EX::isProjectile(enemy)) {
    return 2500 * self.getRole().getAudacity();
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
    lifePriority = -600;
  }

  int statusPriority = 0;
  for (const auto& status : livingUnit.getStatuses()) {
    if (status.getType() == model::STATUS_EMPOWERED) {
      statusPriority += 400;
    }

    if (status.getType() == model::STATUS_FROZEN) {
      statusPriority -= status.getRemainingDurationTicks() * 15;
    }
  }

  if (EX::isWizard(livingUnit)) {
    const model::Wizard& wizard = EX::asWizard(livingUnit);
    const double timeToAttack = EX::timeToTurnForAttack(self, wizard);
    const double timeForMagic = EX::minTimeForMagic(EX::asWizard(livingUnit));

    return (300 + statusPriority + lifePriority - timeForMagic * 5 - timeToAttack * 5) * self.getRole().getAudacity();
  } else if (EX::isMinion(livingUnit)) {
    const model::Minion& minion = EX::asMinion(livingUnit);

    if (model::MINION_ORC_WOODCUTTER == minion.getType()) {
      if (distance < constants.getOrcWoodcutterAttackRange() + self.getRadius() * 2) {
        return (600 + lifePriority)  * self.getRole().getAudacity();
      }
      return (lifePriority + statusPriority) * self.getRole().getAudacity();
    } else {
      return (lifePriority + statusPriority + ((600 * minion.getRemainingActionCooldownTicks()) / minion.getCooldownTicks())) * self.getRole().getAudacity();
    }
  } else if (EX::isBuilding(livingUnit)) {
    return (800 + lifePriority) * self.getRole().getAudacity();
  }

  return 0;
}

int MovePriorities::defendPoint(const Wizard&, const Position&) {
  return 0;
}

int MovePriorities::follow(const Wizard&, const model::LivingUnit&) {
  return 300;
}

int MovePriorities::getExpirience(const Wizard& wizard, const model::LivingUnit&) {
  return 100 * wizard.getRole().getImportanceOfXP();
}

int MovePriorities::keepDistance(const Wizard&, const Position, const double, const double) {
  return 500;
}

int MovePriorities::moveToBonus(const Wizard& self, const Position&) {
  return (50 + 4 * self.getMaxLife()) * self.getRole().getImportanceOfBonus();
}

int MovePriorities::moveToLine(const Wizard& self, const model::LaneType&) {
  return 20 * self.getRole().getImportanceOfXP();
}

int MovePriorities::moveToPoint(const Wizard&, const Position&) {
  return 10;
}

int MovePriorities::observeMap(const Wizard&) {
  return 0;
}