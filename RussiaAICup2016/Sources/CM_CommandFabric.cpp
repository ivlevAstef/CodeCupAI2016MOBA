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

#include "CM_CommandAttackEnemy.h"
#include "CM_CommandPool.h"

#include "CM_CommandDefendPoint.h"
#include "CM_CommandAvoidEnemy.h"
#include "CM_CommandAvoidAround.h"

using namespace AICup;

MoveCommandPtr CommandFabric::keepDistance(const double x, const double y, const double minDistance, const double maxDistance) const {
  return std::make_shared<CommandKeepDistance>(x, y, minDistance, maxDistance);
}

MoveCommandPtr CommandFabric::moveToPoint(const double x, const double y, const TurnStyle style, const double speedLimit) const {
  return std::make_shared<CommandMoveToPoint>(x, y, style, speedLimit);
}

MoveCommandPtr CommandFabric::moveToLine(const model::LaneType line) const {
  return std::make_shared<CommandMoveToLine>(line);
}

/// подойти чтобы взять руну
MoveCommandPtr CommandFabric::moveToBonus() const {
  return std::make_shared<CommandMoveToBonus>();
}

MoveCommandPtr CommandFabric::follow(const long long unitId, const double minDistance, const double maxDistance) const {
  return std::make_shared<CommandFollow>(unitId, minDistance, maxDistance);
}

MoveCommandPtr CommandFabric::moveGetExpirience() const {
  return std::make_shared<CommandMoveGetExpirience>();
}

MoveCommandPtr CommandFabric::observeMap() const {
  return std::make_shared<CommandObserveMap>();
}

AttackCommandPtr CommandFabric::attack(const long long enemyId) const {
  return std::make_shared<CommandAttackEnemy>(enemyId);
}

AttackCommandPtr CommandFabric::pool(const long long neutralUnitId) const {
  return std::make_shared<CommandPool>(neutralUnitId);
}

MoveCommandPtr CommandFabric::defend(const double x, const double y) const {
  return std::make_shared<CommandDefendPoint>(x, y);
}

MoveCommandPtr CommandFabric::avoidEnemy(const long long unitId) const {
  return std::make_shared<CommandAvoidEnemy>(unitId);
}

MoveCommandPtr CommandFabric::avoidAround() const {
  return std::make_shared<CommandAvoidAround>();
}