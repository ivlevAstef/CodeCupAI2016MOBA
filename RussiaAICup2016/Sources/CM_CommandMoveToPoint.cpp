//
//File: CM_CommandMoveToPoint.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToPoint.h"
#include "CM_Move.h"

using namespace AICup;

CommandMoveToPoint::CommandMoveToPoint(double x, double y): point(x, y) {
}

bool CommandMoveToPoint::check(const model::Wizard& self, model::Move& move) {
  path = Move::path(Position(self.getX(), self.getY()), point, pathLength);

  return path.size() >= 2 && pathLength > 1;
}


void CommandMoveToPoint::execute(const model::Wizard& self, model::Move& move) {
  const auto action = Move::move(self, path);

  move.setSpeed(action.speed);
  move.setStrafeSpeed(action.strafeSpeed);
  move.setTurn(action.turn);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToPoint::visualization(const Visualizator& visualizator) const {
  for (size_t index = 1; index < path.size(); index++) {
    visualizator.line(path[index - 1].x, path[index - 1].y, path[index].x, path[index].y, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR