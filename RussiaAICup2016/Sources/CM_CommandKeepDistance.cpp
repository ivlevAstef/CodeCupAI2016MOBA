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

using namespace AICup;

CommandKeepDistance::CommandKeepDistance(Algorithm::PathFinder& finder, const double x, const double y, const double minDistance, const double maxDistance) :
  MoveCommand(finder), x(x), y(y), minDistance(minDistance), maxDistance(maxDistance) {
}


bool CommandKeepDistance::check(const model::Wizard& self) {
  commandMoveToPoint = nullptr;

  const auto selfPos = Position(self.getX(), self.getY());
  const auto pointPos = Position(x, y);
  const double distance = (selfPos - pointPos).length();

  if (distance < minDistance + (maxDistance - minDistance) * 0.5) {
    /// противоположная точка, точке где находиться объект, и длиной = радиусу обзора
    const auto pos = selfPos + (selfPos - pointPos).normal() * self.getVisionRange();

    commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::BACK_TURN);
  } else if (distance > maxDistance) {
    const auto pos = pointPos;

    commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y);
  } else {
    return false;
  }

  return commandMoveToPoint->check(self);
}

int CommandKeepDistance::priority(const model::Wizard& self) {
  return 0;
}

void CommandKeepDistance::execute(const model::Wizard& self, Result& result) {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->execute(self, result);
}

#ifdef ENABLE_VISUALIZATOR
void CommandKeepDistance::visualization(const Visualizator& visualizator) const {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->visualization(visualizator);

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(x, y, 10, 0x00ffff);
  }
}
#endif // ENABLE_VISUALIZATOR