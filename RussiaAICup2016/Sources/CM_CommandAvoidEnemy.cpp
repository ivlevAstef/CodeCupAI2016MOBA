//
//File: CM_CommandAvoidEnemy.cpp
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//


#include "CM_CommandAvoidEnemy.h"
#include "CM_CommandMoveToPoint.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"

using namespace AICup;

CommandAvoidEnemy::CommandAvoidEnemy(Algorithm::PathFinder& finder, const long long enemyId):
  MoveCommand(finder), enemyId(enemyId) {

}

bool CommandAvoidEnemy::check(const model::Wizard& self) {
  enemy = World::instance().unit(enemyId);
  if (nullptr == enemy) {
    moveToPointCommand = nullptr;
    return false;
  }

  const auto constants = Game::instance().model();

  if (EX::isWizard(*enemy)) {
    const model::Wizard& wizard = EX::asWizard(*enemy);
    const double radius = EX::radiusForGuaranteedHit(wizard);
    const double distance1 = radius - EX::minTimeForMagic(wizard) * constants.getWizardBackwardSpeed()  + self.getRadius();
    const double distance2 = radius - EX::timeToTurnForAttack(self, wizard) * constants.getWizardBackwardSpeed() + self.getRadius();
    distance = MIN(distance1, distance2);

    distance = MAX(distance, constants.getStaffRange() + self.getRadius());

  } else if (EX::isMinion(*enemy)) {
    const model::Minion& minion = EX::asMinion(*enemy);

    if (model::MINION_ORC_WOODCUTTER == minion.getType()) {
      distance = constants.getOrcWoodcutterAttackRange() + self.getRadius() + 100/*на всякий случай*/;
    } else {
      distance = constants.getFetishBlowdartAttackRange() + self.getRadius() + constants.getDartRadius();
    }

  } else if (EX::isBuilding(*enemy)) {
    const model::Building& build = EX::asBuilding(*enemy);

    distance = build.getAttackRange() - build.getRemainingActionCooldownTicks() * constants.getWizardBackwardSpeed() + self.getRadius();
    distance = MAX(distance, build.getRadius() + self.getRadius());

  } else {
    return false;
  }


  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(*enemy);

  const auto currentDistance = (selfPos - unitPos).length();
  if (currentDistance > distance) {
    return false;
  }


  /// противоположная точка, точке где находиться объект
  const auto pos = selfPos + (selfPos - unitPos).normal() * (distance - currentDistance);
  moveToPointCommand = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::BACK_TURN);


  return moveToPointCommand->check(self);
}

void CommandAvoidEnemy::execute(const model::Wizard& self, Result& result) {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->execute(self, result);
  result.priority = MovePriorities::avoidEnemy(self, *enemy);
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidEnemy::visualization(const Visualizator& visualizator) const {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR