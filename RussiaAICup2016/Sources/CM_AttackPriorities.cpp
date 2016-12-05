#include "CM_AttackPriorities.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "A_Attack.h"

using namespace AICup;

/// все функции возращают значение от 0 до 1000
/// на самом деле могут быть меньше нуля, ничего страшного в этом нету - все что меньше 1 отсекается

double AttackPriorities::attackBuild(const Wizard& self, const model::Building& build) {
  const double lifePriority = 500 * (build.getMaxLife() - build.getLife()) / build.getMaxLife();
  const double turnPriority = 15 * Algorithm::timeToTurnForAttack(build, self);
  return 300 + lifePriority - turnPriority;
}

double AttackPriorities::attackMinion(const Wizard& self, const model::Minion& minion) {
  if (Algorithm::isMelee(self, minion)) {
    return 1000;
  }

  double lifePriority = 500 * (minion.getMaxLife() - minion.getLife()) / minion.getMaxLife();
  if (self.damage(model::ACTION_MAGIC_MISSILE) < minion.getLife()) {
    lifePriority = 0;
  }

  if (model::MINION_ORC_WOODCUTTER == minion.getType()) {
    const double range = Game::model().getOrcWoodcutterAttackRange() + self.getRadius();
    const double distance = self.getDistanceTo(minion);
    const double nDistance = MAX(distance, range);

    return 100 + lifePriority + 400 * ((range * range) / (nDistance * nDistance));
  } else if (model::MINION_FETISH_BLOWDART == minion.getType()) {
    return 500 + lifePriority;
  }
  assert(false && "Incorrect minion type...");
  return 0;
}

double AttackPriorities::attackTree(const Wizard& self, const model::Tree& tree) {
  /// если дерево вблизи, то бить его обязательно
  if (Algorithm::isMelee(self, tree)) {
    return 1200;
  }

  const double ticksToRotate = Algorithm::timeToTurnForAttack(tree, self);
  const double ticksToAttack = self.minStaffOrMissileCooldown();
  const double ticksToMove = self.getDistanceTo(tree) / self.maxSpeed();

  /// если бежать до дерева, по времени меньше чем нужно чтобы к нему развернуться или чтобы можно было его атаковать то надо его бить
  if (ticksToMove <= MAX(ticksToAttack, ticksToRotate)) {
    return 1000;
  }

  return 0;
}

double AttackPriorities::attackWizard(const Wizard& self, const model::Wizard& wizard) {
  if (Algorithm::isMelee(self, wizard)) {
    return 1000;
  }

  /// если хп (с запасом если немного востановится) меньше атаки, то надо добивать
  if (wizard.getLife() + 1 < self.damage(model::ACTION_MAGIC_MISSILE)) {
    return 2500;
  }

  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = wizardPos - selfPos;

  const auto bulletSpeed = delta.normal() * Game::model().getMagicMissileSpeed();
  /// если маг может уклонится от снаряда, то снижаем приоритет
  double dodgePriority = 1;
  if (Algorithm::canSideForwardEscape(selfPos, self.getCastRange(), wizard, bulletSpeed, 2 * Game::model().getMagicMissileRadius())) {
    dodgePriority *= 0.5;
  }
  if (Algorithm::canSideBackwardEscape(selfPos, self.getCastRange(), wizard, bulletSpeed, 2 * Game::model().getMagicMissileRadius())) {
    dodgePriority *= 0.5;
  }


  const double lifePriority = 500 * (100 - MIN(100, wizard.getLife())) / 100;

  double statusPriority = 0;
  for (const auto& status : wizard.getStatuses()) {
    switch (status.getType()) {
      case model::STATUS_BURNING:
        statusPriority += 150;
        break;
      case model::STATUS_EMPOWERED:
        statusPriority += 100;
        break;
      case model::STATUS_FROZEN:
        statusPriority += 500;
        break;
      case model::STATUS_HASTENED:
        statusPriority -= 50;
        break;
      case model::STATUS_SHIELDED:
        statusPriority -= 300;
        break;
      default:
        break;
    }
  }

  const double distance = self.getDistanceTo(wizard);
  const double distancePriority = 75 * ((500 * 500) / (distance * distance));


  return MAX((lifePriority + statusPriority + distancePriority) * dodgePriority, 1000);
}

double AttackPriorities::attackFrostbolt(const Wizard& self) {
  return 2000;
}
double AttackPriorities::attackFireball(const Wizard& self) {
  return 1500;
}

double AttackPriorities::pool(const Wizard&, const model::Minion&) {
  return 0;
}