//
//File: CM_CommandFabric.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "CM_CommandFabric.h"

#include "CM_CommandKeepDistance.h"
#include "CM_CommandMoveToPoint.h"
#include "CM_CommandMoveToLine.h"
#include "CM_CommandFollow.h"
#include "CM_CommandMoveGetExpirience.h"
#include "CM_CommandMoveToBonus.h"
#include "CM_CommandObserveMap.h"

#include "CM_CommandAttackBuild.h"
#include "CM_CommandAttackMinion.h"
#include "CM_CommandAttackWizard.h"
#include "CM_CommandAttackTree.h"
#include "CM_CommandAttackFrostbolt.h"
#include "CM_CommandAttackFireball.h"
#include "CM_CommandPool.h"
#include "CM_CommandFollowAttackEnemy.h"

#include "CM_CommandDefendPoint.h"
#include "CM_CommandAvoidWizard.h"
#include "CM_CommandAvoidBuild.h"
#include "CM_CommandAvoidMinion.h"
#include "CM_CommandAvoidProjectile.h"

#include "C_Extensions.h"
#include <cassert>

using namespace AICup;

CommandFabric::CommandFabric(Algorithm::PathFinder& finder):
  finder(finder) {

}

MoveCommandPtr CommandFabric::keepDistance(const double x, const double y, const double minDistance, const double maxDistance) const {
  return std::make_shared<CommandKeepDistance>(finder, x, y, minDistance, maxDistance);
}

MoveCommandPtr CommandFabric::moveToPoint(const double x, const double y, const TurnStyle style, const double speedLimit) const {
  return std::make_shared<CommandMoveToPoint>(finder, x, y, style, speedLimit);
}

MoveCommandPtr CommandFabric::moveToLine(const model::LaneType line) const {
  return std::make_shared<CommandMoveToLine>(finder, line);
}

/// подойти чтобы взять руну
MoveCommandPtr CommandFabric::moveToBonus() const {
  return std::make_shared<CommandMoveToBonus>(finder);
}

MoveCommandPtr CommandFabric::follow(const long long unitId, const double minDistance, const double maxDistance) const {
  return std::make_shared<CommandFollow>(finder, unitId, minDistance, maxDistance);
}

MoveCommandPtr CommandFabric::followAttack(const model::Wizard& wizard) const {
  return std::make_shared<CommandFollowAttack>(finder, wizard);
}

MoveCommandPtr CommandFabric::moveGetExpirience() const {
  return std::make_shared<CommandMoveGetExpirience>(finder);
}

MoveCommandPtr CommandFabric::observeMap() const {
  return std::make_shared<CommandObserveMap>(finder);
}

AttackCommandPtr CommandFabric::attack(const model::LivingUnit& unit) const {
  if (EX::isMinion(unit)) {
    return std::make_shared<CommandAttackMinion>(EX::asMinion(unit));
  } if (EX::isWizard(unit)) {
    return std::make_shared<CommandAttackWizard>(EX::asWizard(unit));
  } if (EX::isTree(unit)) {
    return std::make_shared<CommandAttackTree>(EX::asTree(unit));
  } if (EX::isBuilding(unit)) {
    return std::make_shared<CommandAttackBuild>(EX::asBuilding(unit));
  }
  assert(false && "incorrect unit type");
  return nullptr;
}

AttackCommandPtr CommandFabric::attackUseFrostbolt() const {
  return std::make_shared<CommandAttackFrostbolt>();
}
AttackCommandPtr CommandFabric::attackUseFireball() const {
  return std::make_shared<CommandAttackFireball>();
}

AttackCommandPtr CommandFabric::pool(const long long neutralUnitId) const {
  return std::make_shared<CommandPool>(neutralUnitId);
}

MoveCommandPtr CommandFabric::defend(const double x, const double y) const {
  return std::make_shared<CommandDefendPoint>(finder, x, y);
}

MoveCommandPtr CommandFabric::avoidEnemy(const model::LivingUnit& unit) const {
  if (EX::isMinion(unit)) {
    return std::make_shared<CommandAvoidMinion>(finder, EX::asMinion(unit));
  } if (EX::isWizard(unit)) {
    return std::make_shared<CommandAvoidWizard>(finder, EX::asWizard(unit));
  } if (EX::isBuilding(unit)) {
    return std::make_shared<CommandAvoidBuild>(finder, EX::asBuilding(unit));
  }
  assert(false && "incorrect unit type");
  return nullptr;
}

// /уворот от снаряда
MoveCommandPtr CommandFabric::avoidProjectile(const model::Projectile& projectile) const {
  return std::make_shared<CommandAvoidProjectile>(finder, projectile);
}