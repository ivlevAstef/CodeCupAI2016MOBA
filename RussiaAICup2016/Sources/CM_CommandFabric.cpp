//
//File: CM_CommandFabric.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "CM_CommandFabric.h"

#include "CM_CommandMoveToPoint.h"
#include "CM_CommandMoveToLine.h"
#include "CM_CommandFollow.h"
#include "CM_CommandMoveGetExpirience.h"
#include "CM_CommandMoveToBonus.h"

#include "CM_CommandAttackBuild.h"
#include "CM_CommandAttackMinion.h"
#include "CM_CommandAttackWizard.h"
#include "CM_CommandAttackTree.h"
#include "CM_CommandAttackFrostbolt.h"
#include "CM_CommandAttackFireball.h"
#include "CM_CommandPool.h"
#include "CM_CommandFollowAttackEnemy.h"
#include "CM_CommandMoveMeleeAttack.h"

#include "CM_CommandAvoidWizard.h"
#include "CM_CommandAvoidBuild.h"
#include "CM_CommandAvoidMinion.h"
#include "CM_CommandAvoidProjectile.h"

#include "CM_CommandCastHast.h"
#include "CM_CommandCastShield.h"

#include "C_Extensions.h"
#include <cassert>

using namespace AICup;

CommandFabric::CommandFabric(Algorithm::PathFinder& finder):
  finder(finder) {

}

MoveCommandPtr CommandFabric::moveToPoint(const double x, const double y) const {
  return std::make_shared<CommandMoveToPoint>(x, y);
}

MoveCommandPtr CommandFabric::moveToLine(const model::LaneType line) const {
  return std::make_shared<CommandMoveToLine>(line);
}

/// подойти чтобы взять руну
MoveCommandPtr CommandFabric::moveToBonus() const {
  return std::make_shared<CommandMoveToBonus>(finder);
}

MoveCommandPtr CommandFabric::follow(const long long unitId, const double minDistance, const double maxDistance) const {
  return std::make_shared<CommandFollow>(unitId, minDistance, maxDistance);
}

MoveCommandPtr CommandFabric::followAttack(const model::Wizard& wizard) const {
  return std::make_shared<CommandFollowAttack>(wizard);
}

MoveCommandPtr CommandFabric::moveGetExpirience() const {
  return std::make_shared<CommandMoveGetExpirience>();
}

MoveCommandPtr CommandFabric::moveMeleeAttack(const model::LivingUnit& unit) const {
  return std::make_shared<CommandMoveMeleeAttack>(unit);
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

MoveCommandPtr CommandFabric::avoidEnemy(const model::LivingUnit& unit) const {
  if (EX::isMinion(unit)) {
    return std::make_shared<CommandAvoidMinion>(EX::asMinion(unit));
  } if (EX::isWizard(unit)) {
    return std::make_shared<CommandAvoidWizard>(EX::asWizard(unit));
  } if (EX::isBuilding(unit)) {
    return std::make_shared<CommandAvoidBuild>(EX::asBuilding(unit));
  }
  assert(false && "incorrect unit type");
  return nullptr;
}

// /уворот от снаряда
MoveCommandPtr CommandFabric::avoidProjectile(const Bullet& projectile, const Vector& moveDirection) const {
  return std::make_shared<CommandAvoidProjectile>(projectile, moveDirection);
}

CastCommandPtr CommandFabric::haste() const {
  return std::make_shared<CommandCastHast>();
}
CastCommandPtr CommandFabric::shield() const {
  return std::make_shared<CommandCastShield>();
}