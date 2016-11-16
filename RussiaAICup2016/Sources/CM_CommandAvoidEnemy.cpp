//
//File: CM_CommandAvoidEnemy.cpp
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//


#include "CM_CommandAvoidEnemy.h"
#include "CM_CommandFollow.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"

using namespace AICup;

CommandAvoidEnemy::CommandAvoidEnemy(const long long enemyId): enemyId(enemyId) {

}

bool CommandAvoidEnemy::check(const model::Wizard& self) {
  enemy = World::instance().unit(enemyId);
  if (nullptr == enemy) {
    followCommand = nullptr;
    return false;
  }

  const auto constants = Game::instance().model();

  wizardEnemy = dynamic_cast<const model::Wizard*>(enemy);
  minionEnemy = dynamic_cast<const model::Minion*>(enemy);
  buildEnemy = dynamic_cast<const model::Building*>(enemy);

  if (nullptr != wizardEnemy) {
    const double radius = Extension::radiusForGuaranteedHit(*wizardEnemy);
    const double distance1 = radius - Extension::minTimeForMagic(*wizardEnemy) * constants.getWizardBackwardSpeed()  + self.getRadius();
    const double distance2 = radius - Extension::timeToTurnForAttack(self, *wizardEnemy) * constants.getWizardBackwardSpeed() + self.getRadius();
    distance = MIN(distance1, distance2);

    distance = MAX(distance, constants.getStaffRange() + self.getRadius());
  } else if (nullptr != minionEnemy) {
    if (model::MINION_ORC_WOODCUTTER == minionEnemy->getType()) {
      distance = constants.getOrcWoodcutterAttackRange() + self.getRadius() + 100/*на всякий случай*/;
    } else {
      distance = constants.getFetishBlowdartAttackRange() + self.getRadius() + constants.getDartRadius();
    }
  } else if (nullptr != buildEnemy) {
    distance = buildEnemy->getAttackRange() - buildEnemy->getRemainingActionCooldownTicks() * constants.getWizardBackwardSpeed() + self.getRadius();
    distance = MAX(distance, buildEnemy->getRadius() + self.getRadius());
  } else {
    return false;
  }

  followCommand = std::make_shared<CommandFollow>(enemyId, distance, distance + self.getRadius());

  return true;
}

int CommandAvoidEnemy::priority(const model::Wizard& self) {
  assert(nullptr != followCommand.get());

  const auto constants = Game::instance().model();

  const auto selfPos = Position(self.getX(), self.getY());
  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const double distance = (selfPos - enemyPos).length();

  int lifePriority = (200 * enemy->getLife()) / enemy->getMaxLife();

  /// если врага можно быстро добить, то боятся его стоит меньше
  if (enemy->getLife() < Extension::magicMissleAttack(self) * 2) {
    lifePriority = -500;
  }

  if (nullptr != wizardEnemy) {
    const int veryNearPrior = (distance < constants.getStaffRange() + self.getRadius()) ? 400 : 0;
    return 300 + lifePriority - Extension::minTimeForMagic(*wizardEnemy) * 5 + veryNearPrior;
  } else if (nullptr != minionEnemy) {
    if (distance < constants.getOrcWoodcutterAttackRange() + self.getRadius() * 2) {
      return 800 + lifePriority;
    }
    return 150 + lifePriority;
  } else if (nullptr != buildEnemy) {
    return 800 + lifePriority;
  }

  return 0;
}


void CommandAvoidEnemy::execute(const model::Wizard& self, model::Move& move) {
  assert(nullptr != followCommand.get());
  followCommand->execute(self, move);
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidEnemy::visualization(const Visualizator& visualizator) const {
  assert(nullptr != followCommand.get());
  followCommand->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR