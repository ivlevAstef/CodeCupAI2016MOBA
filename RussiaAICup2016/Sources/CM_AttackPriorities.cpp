#include "CM_AttackPriorities.h"
#include "E_Game.h"
#include "E_World.h"
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

  double lifePriority = 200 * (minion.getMaxLife() - minion.getLife()) / minion.getMaxLife();

  if (model::MINION_ORC_WOODCUTTER == minion.getType()) {
    const double range = Game::model().getOrcWoodcutterAttackRange() + self.getRadius();
    const double distance = self.getDistanceTo(minion);
    const double nDistance = MAX(distance, range);

    return 200 + lifePriority + 400 * ((range * range) / (nDistance * nDistance));
  } else if (model::MINION_FETISH_BLOWDART == minion.getType()) {
    return 300 + lifePriority;
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

double AttackPriorities::attackWizard(const Wizard& self, const model::Wizard& wizard, const Bullet* bullet) {
  if (Algorithm::isMelee(self, wizard)) {
    return 1500;
  }

  /// если хп (с запасом если немного востановится) меньше атаки, то надо добивать
  if (wizard.getLife() + 1 < self.damage(model::ACTION_MAGIC_MISSILE)) {
    return 2500;
  }

  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = wizardPos - selfPos;

  /// если маг может уклонится от снаряда, то снижаем приоритет
  double dodgePriority = 1;
  if (nullptr != bullet && Algorithm::canDodge(wizard, Vector(1, 0).rotate(wizard.getAngle()), *bullet)) {
    dodgePriority = 0.25;
  }


  double supportedPriority = 0;
  /// если маги вокруге могут теоретически атаковать, то повышаем приоритет
  for (const auto& aroundWizard : World::model().getWizards()) {
    if (self.getFaction() == aroundWizard.getFaction() && self.getId() != aroundWizard.getId()
      && aroundWizard.getDistanceTo(wizard) < EX::radiusForGuaranteedDodge(wizard, 0)) {
      supportedPriority += 250;
    }
  }

  /// если миньоны вокруге могут теоретически атаковать, то повышаем приоритет
  for (const auto& around : World::instance().aroundEnemies(wizard, 400)) {
    if (EX::isMinion(*around)) {
      const auto& minion = EX::asMinion(*around);
      double distance = minion.getDistanceTo(wizard);
      if (model::MINION_ORC_WOODCUTTER == minion.getType()
        && distance < Game::model().getOrcWoodcutterAttackRange() + wizard.getRadius()) {
        supportedPriority += 100;
      } else if (model::MINION_FETISH_BLOWDART == minion.getType()
        && distance < Game::model().getFetishBlowdartAttackRange()) {
        supportedPriority += 50;
      }
    }
  }


  const double lifePriority = 100.0 + 400.0 * (1.0 - MIN(100.0, double(wizard.getLife()))/100.0);

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
        statusPriority += 750;
        break;
      case model::STATUS_HASTENED:
        statusPriority -= 150;
        break;
      case model::STATUS_SHIELDED:
        statusPriority -= 300;
        break;
      default:
        break;
    }
  }

  const double distance = self.getDistanceTo(wizard);
  const double distancePriority = 500 * ((500 * 500) / (distance * distance));

  return MIN((lifePriority + statusPriority + distancePriority + supportedPriority) * dodgePriority, 2200);
}

double AttackPriorities::attackFrostbolt(const Wizard& self) {
  return 3000;
}
double AttackPriorities::attackFireball(const Wizard& self) {
  return 2500;
}

double AttackPriorities::pool(const Wizard&, const model::Minion&) {
  return 0;
}