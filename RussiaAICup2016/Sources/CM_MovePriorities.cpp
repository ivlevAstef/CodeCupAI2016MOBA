#include "CM_MovePriorities.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "A_Attack.h"

using namespace AICup;

///слегка выше
double MovePriorities::avoidBuild(const Wizard& self, const model::Building& build) {
  const int lifePriority = (300 * build.getLife()) / build.getMaxLife();
  return 800 + lifePriority;
}

double MovePriorities::avoidMinion(const Wizard& self, const model::Minion& minion) {
  const auto mc = Game::model();

  const auto selfPos = EX::pos(self);
  const auto enemyPos = EX::pos(minion);
  const double distance = (selfPos - enemyPos).length();

  const double lifePriority = (300 * minion.getLife()) / minion.getMaxLife();

  if (model::MINION_ORC_WOODCUTTER == minion.getType()) {
    if (distance < mc.getOrcWoodcutterAttackRange() + self.getRadius() + 50/*небольшой запас*/) {
      return 900 + lifePriority;
    }
    return 450 + lifePriority;
  } else {
    return 2*lifePriority + ((400 * minion.getRemainingActionCooldownTicks()) / minion.getCooldownTicks());
  }
}

double MovePriorities::avoidWizard(const Wizard& self, const model::Wizard& wizard) {
  const double timeToTurnAttack = Algorithm::timeToTurnForAttack(self, wizard);
  const double timeForMagic = EX::minTimeForMagic(wizard);

  /// если мага можно быстро добить, то его не стоит бояться
  int lifePriority = (600 * wizard.getLife()) / wizard.getMaxLife();

  if (wizard.getLife() + 2/*так как есть регенерация*/ < 2 * EX::magicMissleAttack(self)) {
    lifePriority -= 200 * EX::magicMissleAttack(self);
  }


  int statusPriority = 0;
  for (const auto& status : wizard.getStatuses()) {
    if (model::STATUS_EMPOWERED == status.getType()) {
      statusPriority += 400;
    } else if (model::STATUS_FROZEN == status.getType()) {
      statusPriority -= status.getRemainingDurationTicks() * 100;
    }
  }

  int skillPriority = 0;
  if (EX::availableSkill(wizard, model::ACTION_FIREBALL)) {
    skillPriority = 3 * (Game::model().getFireballCooldownTicks() - EX::cooldownMaxSkill(wizard, model::ACTION_FIREBALL));
  }

  if (EX::availableSkill(wizard, model::ACTION_FROST_BOLT)) {
    skillPriority = 5 * (Game::model().getFrostBoltCooldownTicks() - EX::cooldownMaxSkill(wizard, model::ACTION_FROST_BOLT));
  }


  return MAX(1000, 500 + statusPriority + skillPriority + lifePriority - timeForMagic * 10 - timeToTurnAttack * 25);
}

double MovePriorities::attackFollow(const Wizard& self, const model::Wizard& wizard) {
  return 450;
}

/// Это очень важно, поэтому завышено значение от нормы
/// Вариативность нужна чтобы каждый тик, выбирался один и тотже projectile из доступных, а не случайный
double MovePriorities::avoidProjectile(const Wizard& self, const model::Projectile& projectile) {
  return 15000 - self.getDistanceTo(projectile);
}

double MovePriorities::defendPoint(const Wizard&, const Position&) {
  return 0;
}

double MovePriorities::follow(const Wizard&, const model::LivingUnit&) {
  return 300;
}

double MovePriorities::getExpirience(const Wizard& wizard, const model::LivingUnit&) {
  return 350 * wizard.getRole().getImportanceOfXP();
}

double MovePriorities::keepDistance(const Wizard&, const Position, const double, const double) {
  return 500;
}

double MovePriorities::moveToBonus(const Wizard& self, const Position&) {
  return (350 + 5 * self.getLife()) * self.getRole().getImportanceOfBonus();
}

double MovePriorities::moveToLine(const Wizard& self, const model::LaneType&) {
  const auto lifeC = (self.getMaxLife() - self.getLife()) / self.getMaxLife();
  return (250 + 1500 * lifeC) * self.getRole().getImportanceOfXP();
}

double MovePriorities::moveToPoint(const Wizard&, const Position&) {
  return 10;
}

double MovePriorities::observeMap(const Wizard&) {
  return 0;
}