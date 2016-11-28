#include "CM_AttackPriorities.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "A_Attack.h"

using namespace AICup;

/// все функции возращают значение от 0 до 1000
/// на самом деле могут быть меньше нуля, ничего страшного в этом нету - все что меньше 1 отсекается

double AttackPriorities::attackBuild(const Wizard& self, const model::Building& build) {
  const double lifePriority = 800 * (build.getMaxLife() - build.getLife()) / build.getMaxLife();
  const double turnPriority = 50 * Algorithm::timeToTurnForAttack(build, self);
  return 200 + lifePriority - turnPriority;
}

double AttackPriorities::attackMinion(const Wizard& self, const model::Minion& minion) {
  const double lifePriority = 500 * (minion.getMaxLife() - minion.getLife()) / minion.getMaxLife();
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
  if (Algorithm::isMelee(self, tree)) {
    return 1000;
  }

  const double ticksToMove = self.getDistanceTo(tree) / self.maxSpeed();
  const double ticksToRemove = tree.getLife() / self.dps(model::ACTION_MAGIC_MISSILE);
  return MAX(100 * (ticksToRemove - ticksToMove), 1000);
}

double AttackPriorities::attackWizard(const Wizard& self, const model::Wizard& wizard) {
  if (Algorithm::isMelee(self, wizard)) {
    return 1000;
  }

  /// если хп (с запасом если немного востановится) меньше атаки, то надо добивать
  if (wizard.getLife() + 1 < self.damage(model::ACTION_MAGIC_MISSILE)) {
    return 1000;
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


  return MAX(lifePriority + statusPriority + distancePriority, 1000);
}

double AttackPriorities::pool(const Wizard&, const model::Minion&) {
  return 0;
}