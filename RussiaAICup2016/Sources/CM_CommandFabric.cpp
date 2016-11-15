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

CommandPtr CommandFabric::keepDistance(const double x, const double y, const double minDistance, const double maxDistance) const {
  return std::make_shared<CommandKeepDistance>(x, y, minDistance, maxDistance);
}

CommandPtr CommandFabric::moveToPoint(const double x, const double y) const {
  return std::make_shared<CommandMoveToPoint>(x, y);
}

CommandPtr CommandFabric::moveToLine(const model::LaneType line) const {
  return std::make_shared<CommandMoveToLine>(line);
}

/// подойти чтобы взять руну
CommandPtr CommandFabric::moveToBonus() const {
  return std::make_shared<CommandMoveToBonus>();
}

CommandPtr CommandFabric::follow(const long long unitId, const double minDistance, const double maxDistance) const {
  return std::make_shared<CommandFollow>(unitId, minDistance, maxDistance);
}

CommandPtr CommandFabric::moveGetExpirience() const {
  return std::make_shared<CommandMoveGetExpirience>();
}

CommandPtr CommandFabric::observeMap() const {
  return std::make_shared<CommandObserveMap>();
}

CommandPtr CommandFabric::attack(const long long enemyId) const {
  return std::make_shared<CommandAttackEnemy>(enemyId);
}

CommandPtr CommandFabric::pool(const long long neutralUnitId) const {
  return std::make_shared<CommandPool>(neutralUnitId);
}

CommandPtr CommandFabric::defend(const double x, const double y) const {
  return std::make_shared<CommandDefendPoint>(x, y);
}

CommandPtr CommandFabric::avoidEnemy(const long long unitId) const {
  return std::make_shared<CommandAvoidEnemy>(unitId);
}

CommandPtr CommandFabric::avoidAround() const {
  return std::make_shared<CommandAvoidAround>();
}