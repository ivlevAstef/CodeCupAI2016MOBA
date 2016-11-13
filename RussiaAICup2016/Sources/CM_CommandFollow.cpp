//
//File: CM_CommandFollow.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandFollow.h"
#include "E_World.h"
#include "CM_Move.h"
#include "C_Math.h"

using namespace AICup;

CommandFollow::CommandFollow(const long long unitId, const double minDistance, const double maxDistance):
  unitId(unitId), minDistance(minDistance), maxDistance(maxDistance) {
}


bool CommandFollow::check(const model::Wizard& self, model::Move& move) {
  const auto unit = World::instance().unit(unitId);
  if (nullptr == unit) {
    commandMoveToPoint = nullptr;
    return false;
  }


  const auto selfPos = Position(self.getX(), self.getY());
  const auto unitPos = Position(unit->getX(), unit->getY());
  const double distance = (selfPos - unitPos).length();

  if (distance < minDistance) {
    /// противоположная точка, точке где находиться объект, и длиной = радиусу обзора
    const auto pos = selfPos + (selfPos - unitPos).normal() * self.getVisionRange();

    commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pos.x, pos.y, SPEED_LIMIT_NOT_SET, MOVE_WITH_BACKWARD_ROTATE);
  } else if (distance > maxDistance) {
    const auto pos = unitPos;

    commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pos.x, pos.y);
  } else {
    const auto currentBackVec = (selfPos - unitPos).normal();
    const auto backVec = Vector(1,0).rotate(unit->getAngle() + AICUP_PI);

    /// находимся за спиной
    if (backVec.dot(currentBackVec) > cos(AICUP_PI / 18)) {
      /// двигаемся к нему но с его предположительной скоростью (по факту разворачиваемся)
      const auto pos = unitPos;
      const double speedLimit = Vector(unit->getSpeedX(), unit->getSpeedY()).length();

      commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pos.x, pos.y, speedLimit);
    } else {
      // двигаться за спину объекта в центр дистанции
      const auto pos = unitPos + backVec * (minDistance + (maxDistance - minDistance) * 0.5);

      commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pos.x, pos.y);
    }
  }


  return commandMoveToPoint->check(self, move);
}

void CommandFollow::execute(const model::Wizard& self, model::Move& move) {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->execute(self, move);
}

#ifdef ENABLE_VISUALIZATOR
void CommandFollow::visualization(const Visualizator& visualizator) const {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR