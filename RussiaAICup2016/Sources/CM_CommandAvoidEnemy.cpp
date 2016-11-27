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

bool CommandAvoidEnemy::check(const Wizard& self) {
  enemy = World::instance().unitOrProjectile(enemyId);
  if (nullptr == enemy) {
    moveToPointCommand = nullptr;
    return false;
  }

  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(*enemy);
  const auto delta = selfPos - unitPos;
  const auto constants = Game::instance().model();

  if (EX::isProjectile(*enemy)) {
    const auto maxDistance = self.getRadius() + enemy->getRadius();
    const auto projectileSpeed = Vector(enemy->getSpeedX(), enemy->getSpeedY());
    const auto devDistance = Math::distanceToLine(selfPos, unitPos, unitPos + projectileSpeed);
    /// нахожусь на лиинии удара, снаряд летит в мою сторону, он близко
    if (devDistance < maxDistance && delta.dot(projectileSpeed) > 0 && delta.length() < 645/*максимально доступная длина*/) {
    } else {
      return false;
    }

    /// противоположная точка, точке где находиться объект
    const auto pos = selfPos + delta.normal() * 120/*приблизительно столько со всеми бонусами максиум можно пройти*/;
    moveToPointCommand = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::TURN);
    return moveToPointCommand->check(self);
  }

  double distance = 0;
  if (EX::isWizard(*enemy)) {
    const model::Wizard& wizard = EX::asWizard(*enemy);
    const double radius = EX::radiusForGuaranteedHit(wizard, *enemy);
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

  const auto currentDistance = delta.length();
  if (currentDistance > distance + 1/*чтобы был запас*/) {
    return false;
  }


  /// противоположная точка, точке где находиться объект
  const auto pos = selfPos + delta.normal() * (distance - currentDistance);
  moveToPointCommand = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::BACK_TURN);

  return moveToPointCommand->check(self);
}

void CommandAvoidEnemy::execute(const Wizard& self, Result& result) {
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