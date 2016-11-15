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

  const auto selfPos = Position(self.getX(), self.getY());
  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const double distance = (selfPos - enemyPos).length();

  wizardEnemy = dynamic_cast<const model::Wizard*>(enemy);
  minionEnemy = dynamic_cast<const model::Minion*>(enemy);
  buildEnemy = dynamic_cast<const model::Building*>(enemy);

  if (nullptr != wizardEnemy) {
    minDistance = wizardEnemy->getCastRange() - wizardEnemy->getRemainingActionCooldownTicks() * constants.getWizardBackwardSpeed()  + self.getRadius();
    minDistance = MAX(minDistance, constants.getStaffRange() + self.getRadius());
  } else if (nullptr != minionEnemy) {
    if (model::MINION_ORC_WOODCUTTER == minionEnemy->getType()) {
      minDistance = constants.getOrcWoodcutterAttackRange() + self.getRadius() * 2;
    } else {
      minDistance = constants.getFetishBlowdartAttackRange() + self.getRadius() + constants.getDartRadius();
    }
  } else if (nullptr != buildEnemy) {
    minDistance = buildEnemy->getAttackRange() - buildEnemy->getRemainingActionCooldownTicks() * constants.getWizardBackwardSpeed() + self.getRadius();
    minDistance = MAX(minDistance, buildEnemy->getRadius() + self.getRadius());
  } else {
    return false;
  }

  maxDistance = minDistance + self.getRadius();
  followCommand = std::make_shared<CommandFollow>(enemyId, minDistance, maxDistance);

  return true;
}

int CommandAvoidEnemy::priority(const model::Wizard& self) {
  assert(nullptr != followCommand.get());

  const auto constants = Game::instance().model();

  const auto selfPos = Position(self.getX(), self.getY());
  const auto enemyPos = Position(enemy->getX(), enemy->getY());
  const double distance = (selfPos - enemyPos).length();

  const int lifePriority = (200 * enemy->getLife()) / enemy->getMaxLife();

  if (nullptr != wizardEnemy) {
    const int veryNearPrior = (distance < constants.getStaffRange() + self.getRadius()) ? 400 : 0;
    return 300 + lifePriority - wizardEnemy->getRemainingActionCooldownTicks() * 5 + veryNearPrior;
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