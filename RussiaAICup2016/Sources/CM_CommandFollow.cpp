//
//File: CM_CommandFollow.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandFollow.h"
#include "E_World.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "C_Math.h"

using namespace AICup;

CommandFollow::CommandFollow(const long long unitId, const double minDistance, const double maxDistance):
  unitId(unitId), minDistance(minDistance), maxDistance(maxDistance) {
  target = nullptr;
}


bool CommandFollow::check(const Wizard&) {
  target = World::instance().unit(unitId);
  return nullptr != target;
}

void CommandFollow::execute(const Wizard& self, Result& result) {
  assert(nullptr != target);

  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(*target);
  const double distance = (selfPos - unitPos).length();

  if (distance < minDistance) {
    /// противоположна€ точка, точке где находитьс€ объект, и длиной = радиусу обзора
    position = selfPos + (selfPos - unitPos).normal() * self.getVisionRange();
  } else if (distance > maxDistance) {
    position = unitPos;
  } else {
    const auto currentBackVec = (selfPos - unitPos).normal();

    /// если это здание - у него нет угла поворота
    if (EX::isBuilding(*target)) {
      // двигатьс€ назад в центр дистанции
      position = unitPos + currentBackVec * (minDistance + (maxDistance - minDistance) * 0.5);

    } else {
      const auto backVec = Vector(1, 0).rotate(target->getAngle() + AICUP_PI);

      /// находимс€ за спиной
      if (backVec.dot(currentBackVec) > cos(AICUP_PI / 18)) {
        const auto speed = Vector(target->getSpeedX(), target->getSpeedY());

        position = unitPos; /// двигаемс€ к нему но с его предположительной скоростью (по факту разворачиваемс€)
                            /// если скорость противоположна углу куда смотрим юнит, значит он двигаетьс€ задом
        if (abs(Math::angleDiff(speed.angle(), target->getAngle())) > AICUP_PI) {
          /// двигаемс€ от него
          position = selfPos + (selfPos - unitPos).normal() * self.getVisionRange();
        }
      } else {
        // двигатьс€ за спину объекта в центр дистанции
        position = unitPos + backVec * (minDistance + (maxDistance - minDistance) * 0.5);
      }
    }
  }

  result.set(position, self);
  result.turnDirection = Vector();

  result.turnPriority = TurnPriority::follow;
  result.priority = MovePriorities::follow(self, *target);
}

#ifdef ENABLE_VISUALIZATOR
void CommandFollow::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != target);
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);
    const auto to = EX::pos(*target);

    visualizator.line(from.x, from.y, to.x, to.y, 0x00ff00);
  }
}
#endif // ENABLE_VISUALIZATOR