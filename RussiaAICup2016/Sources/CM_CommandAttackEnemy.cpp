//
//File: CM_CommandAttackEnemy.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandAttackEnemy.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"

using namespace AICup;

CommandAttackEnemy::CommandAttackEnemy(long long enemyId): enemyId(enemyId) {

}

bool CommandAttackEnemy::check(const model::Wizard& self) {
  const auto enemy = World::instance().unit(enemyId);
  if (nullptr == enemy) {
    return false;
  }

  selfPos = Position(self.getX(), self.getY());
  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const double distance = (selfPos - enemyPos).length();

  /// если враг совсем далеко, то атака невозможна
  if (distance > EX::radiusForGuaranteedHit(self)) {
    return false;
  }

  /// если времени до атаки больше чем времени до разворота, то можно пока не атаковать
  if (self.getRemainingActionCooldownTicks() > EX::timeToTurnForAttack(*enemy, self)) {
    return false;
  }

  /// если на пути есть дерево, то не стоит атаковать
  for (const auto& tree : World::instance().trees()) {
    if (tree.getId() == enemy->getId()) {
      continue;
    }

    if (Math::distanceToSegment(EX::pos(tree), selfPos, enemyPos) < tree.getRadius() + 10/*радиус патрона*/) {
      return false;
    }
  }

  /// во всех остальных случаях возможна или атака, или поворот к юниту для атаки
  return true;
}

int CommandAttackEnemy::priority(const model::Wizard& self) {
  const auto enemy = World::instance().unit(enemyId);
  if (nullptr == enemy) {
    return 0;
  }

  const auto constants = Game::model();

  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const double distance = (selfPos - enemyPos).length();

  double lifePriority = double(enemy->getMaxLife()-enemy->getLife()) / double(enemy->getMaxLife());

  const auto wizardEnemy = dynamic_cast<const model::Wizard*>(enemy);
  const auto minionEnemy = dynamic_cast<const model::Minion*>(enemy);
  const auto buildEnemy = dynamic_cast<const model::Building*>(enemy);
  const auto treeEnemy = dynamic_cast<const model::Tree*>(enemy);

  /// если хп мало у обычного крипа, то очков мне дадут меньше, пускай другие добивают
  if (enemy->getLife() < constants.getMagicMissileDirectDamage() && nullptr != minionEnemy) {
    lifePriority *= 0;
  }

  /// если хп мало у мага, то надо ему помочь умереть
  if (enemy->getLife() <= EX::magicMissleAttack(self) && nullptr != wizardEnemy) {
    lifePriority *= 6;
  }

  double typePriority = 0;
  if (nullptr != wizardEnemy) {
    typePriority = 1;
  } else if (nullptr != minionEnemy) {
    const double range = constants.getOrcWoodcutterAttackRange() + self.getRadius();

    switch (minionEnemy->getType()) {
      case model::MINION_ORC_WOODCUTTER:
        typePriority = 0.2 + 0.7 * (range*range / (distance*distance));
        break;
      case model::MINION_FETISH_BLOWDART:
        typePriority = 0.5;
        break;
      default:
        break;
    }
  } else if (nullptr != buildEnemy) {
    typePriority = 0.2 + lifePriority * 0.7;
  } else if (nullptr != treeEnemy) {
    typePriority = 2 * (1 - distance / self.getVisionRange());
  }

  double statusPriority = 0.5;
  for (const auto& status : enemy->getStatuses()) {
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


  return 1000 * (0.3 * lifePriority + 0.5 * typePriority + 0.2 * statusPriority);
}

void CommandAttackEnemy::execute(const model::Wizard& self, Result& result) {
  const auto enemy = World::instance().unit(enemyId);
  assert(nullptr != enemy);

  const double distance = self.getDistanceTo(*enemy);

  result.unit = enemy;
  result.priority = priority(self);

  if (distance < Game::model().getStaffRange() + enemy->getRadius() - EX::maxSpeed(*enemy)) {
    result.action = model::ACTION_STAFF;
  } else {
    result.action = model::ACTION_MAGIC_MISSILE;
  }
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackEnemy::visualization(const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto enemy = World::instance().unit(enemyId);
    assert(nullptr != enemy);

    visualizator.line(selfPos.x, selfPos.y, enemy->getX(), enemy->getY(), 0xff0000);
    visualizator.fillCircle(enemy->getX(), enemy->getY(), enemy->getRadius() * 0.75, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR