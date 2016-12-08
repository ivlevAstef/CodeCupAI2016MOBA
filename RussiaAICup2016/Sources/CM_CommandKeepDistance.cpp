//
//File: CM_CommandKeepDistance.cpp
//Author: Ivlev Alexander. Stef
//Created: 15/11/2016
//


#include "CM_CommandKeepDistance.h"
#include "CM_CommandMoveToPoint.h"
#include "E_World.h"
#include "A_Move.h"
#include "C_Math.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "C_Extensions.h"

using namespace AICup;

CommandKeepDistance::CommandKeepDistance(const double x, const double y, const double minDistance, const double maxDistance) :
  x(x), y(y), minDistance(minDistance), maxDistance(maxDistance) {
}


bool CommandKeepDistance::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto pointPos = Position(x, y);
  const double distance = (selfPos - pointPos).length();

  if (distance < minDistance + (maxDistance - minDistance) * 0.5) {
    return true;
  } else if (distance > maxDistance) {
    return true;
  }

  return false;
}

void CommandKeepDistance::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto pointPos = Position(x, y);
  const double distance = (selfPos - pointPos).length();


  auto turnStyle = TurnStyle::TURN;
  if (distance < minDistance + (maxDistance - minDistance) * 0.5) {
    /// противоположная точка, точке где находиться объект, и длиной = радиусу обзора
    position = selfPos + (selfPos - pointPos).normal() * self.getVisionRange();
    turnStyle = TurnStyle::BACK_TURN;
  } else if (distance > maxDistance) {
    position = pointPos;
  } else {
    result.priority = -1000; /// чтото явно пошло не так
    return;
  }

  result.set(position, self);
  result.turnStyle = turnStyle;
  result.turnPriority = TurnPriority::keepDistance;
  result.priority = MovePriorities::keepDistance(self, Position(x, y), minDistance, maxDistance);
}


#ifdef ENABLE_VISUALIZATOR
void CommandKeepDistance::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, position.x, position.y, 0x0000ff);
  }
}
#endif // ENABLE_VISUALIZATOR