#include "CM_AttackPriorities.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "C_Math.h"

using namespace AICup;

int AttackPriorities::attackEnemy(const model::Wizard& self, const model::LivingUnit& enemy) {
  const auto constants = Game::model();

  const auto selfPos = EX::pos(self);
  const auto enemyPos = EX::pos(enemy);
  const double distance = (selfPos - enemyPos).length();

  double lifePriority = double(enemy.getMaxLife() - enemy.getLife()) / double(enemy.getMaxLife());

  /// если хп мало у обычного крипа, то очков мне дадут меньше, пускай другие добивают
  if (enemy.getLife() < constants.getMagicMissileDirectDamage() && EX::isMinion(enemy)) {
    lifePriority *= 0;
  }

  /// если хп мало у мага, то надо ему помочь умереть
  if (enemy.getLife() <= EX::magicMissleAttack(self) && EX::isWizard(enemy)) {
    lifePriority *= 6;
  }

  double typePriority = 0;
  if (EX::isWizard(enemy)) {
    typePriority = 1;
  } else if (EX::isMinion(enemy)) {
    const double range = constants.getOrcWoodcutterAttackRange() + self.getRadius();

    switch (EX::asMinion(enemy).getType()) {
      case model::MINION_ORC_WOODCUTTER:
        typePriority = 0.2 + 0.7 * (range*range / (distance*distance));
        break;
      case model::MINION_FETISH_BLOWDART:
        typePriority = 0.5;
        break;
      default:
        break;
    }
  } else if (EX::isBuilding(enemy)) {
    typePriority = 0.2 + lifePriority * 0.7;
  } else if (EX::isTree(enemy)) {
    typePriority = 2 * (1 - distance / self.getVisionRange());
  }

  double statusPriority = 0.5;
  for (const auto& status : enemy.getStatuses()) {
    switch (status.getType()) {
      case model::STATUS_BURNING:
        statusPriority += 0.1;
        break;
      case model::STATUS_EMPOWERED:
        statusPriority += 0.2;
        break;
      case model::STATUS_FROZEN:
        statusPriority += 1.0;
        break;
      case model::STATUS_HASTENED:
        statusPriority -= 0.3;
        break;
      case model::STATUS_SHIELDED:
        statusPriority -= 1.0;
        break;
      default:
        break;
    }
  }
  statusPriority = MAX(-1, MIN(statusPriority, 1));


  return int(1000 * (0.3 * lifePriority + 0.5 * typePriority + 0.2 * statusPriority));
}

int AttackPriorities::pool(const model::Wizard& self, const model::Minion& neutral) {
  return 0;
}