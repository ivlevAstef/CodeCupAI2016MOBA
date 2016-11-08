//
//File: CM_CommandFabric.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "CM_CommandFabric.h"

#include "CM_CommandMoveToPoint.h"
#include "CM_CommandMoveToLine.h"
#include "CM_CommandFollow.h"
#include "CM_CommandObserveMap.h"

#include "CM_CommandAttackEnemy.h"
#include "CM_CommandPool.h"
#include "CM_CommandDefendPoint.h"

using namespace AICup;

CommandPtr CommandFabric::moveToPoint(double x, double y) {
  return std::make_shared<CommandMoveToPoint>(x, y);
}

CommandPtr CommandFabric::moveToLine(model::LineType line) {
  return std::make_shared<CommandMoveToLine>(line);
}

CommandPtr CommandFabric::follow(long long unitId) {
  return std::make_shared<CommandFollow>(unitId);
}

CommandPtr CommandFabric::observeMap() {
  return std::make_shared<CommandObserveMap>();
}

CommandPtr CommandFabric::attack(long long enemyId) {
  return std::make_shared<CommandAttackEnemy>(enemyId);
}

CommandPtr CommandFabric::pool(long long neutralUnitId) {
  return std::make_shared<CommandPool>(neutralUnitId);
}

CommandPtr CommandFabric::defend(double x, double y) {
  return std::make_shared<CommandDefendPoint>(x, y);
}


