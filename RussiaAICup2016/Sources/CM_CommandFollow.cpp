//
//File: CM_CommandFollow.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandFollow.h"
#include "CM_CommandMoveToPoint.h"
#include "E_World.h"
#include "A_Move.h"
#include "C_Math.h"

using namespace AICup;

CommandFollow::CommandFollow(const long long unitId, const double minDistance, const double maxDistance):
  unitId(unitId), minDistance(minDistance), maxDistance(maxDistance) {
}


bool CommandFollow::check(const model::Wizard& self) {
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

    commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pos.x, pos.y, TurnStyle::BACK_TURN);
  } else if (distance > maxDistance) {
    const auto pos = unitPos;

    commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pos.x, pos.y);
  } else {
    const auto currentBackVec = (selfPos - unitPos).normal();

    /// если это здание
    if (nullptr != dynamic_cast<const model::Building*>(unit)) {
      // двигаться назад в центр дистанции
      const auto pos = unitPos + currentBackVec * (minDistance + (maxDistance - minDistance) * 0.5);

      commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pos.x, pos.y);
    } else {
      const auto backVec = Vector(1, 0).rotate(unit->getAngle() + AICUP_PI);

      /// находимся за спиной
      if (backVec.dot(currentBackVec) > cos(AICUP_PI / 18)) {

        const auto speed = Vector(unit->getSpeedX(), unit->getSpeedY());

        auto pos = unitPos; /// двигаемся к нему но с его предположительной скоростью (по факту разворачиваемся)
                            /// если скорость противоположна углу куда смотрим юнит, значит он двигаеться задом
        if (abs(Math::angleDiff(speed.angle(), unit->getAngle())) > AICUP_PI) {
          /// двигаемся от него
          pos = selfPos + (selfPos - unitPos).normal() * self.getVisionRange();
        }


        commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pos.x, pos.y, TurnStyle::TURN, speed.length());
      } else {
        // двигаться за спину объекта в центр дистанции
        const auto pos = unitPos + backVec * (minDistance + (maxDistance - minDistance) * 0.5);

        commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pos.x, pos.y);
      }
    }
  }


  return commandMoveToPoint->check(self);
}

int CommandFollow::priority(const model::Wizard& self) {
  return 0;
}

void CommandFollow::execute(const model::Wizard& self, Result& result) {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->execute(self, result);
}

#ifdef ENABLE_VISUALIZATOR
void CommandFollow::visualization(const Visualizator& visualizator) const {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR