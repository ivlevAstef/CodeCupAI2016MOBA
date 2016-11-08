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

bool CommandMoveToPoint::execute(const model::Wizard& self, model::Move& move) {
  double newPathLength = 0;
  const auto newPath = Move::path(Position(self.getX(), self.getY()), point, newPathLength);

  if (abs(newPathLength - pathLength) > 50) {
    pathLength = newPathLength;
    path = newPath;
  }

  if (path.size() < 2) {
    return false;
  }



  const auto action = Move::move(MovableUnit{self.getX(), self.getY(), self.getAngle()}, path);

  move.setSpeed(action.speed);
  move.setStrafeSpeed(action.strafeSpeed);
  move.setTurn(action.turn);

  return true;
}

const std::vector<CommandPtr>& CommandMoveToPoint::neededCommands(const CommandFabric& fabric) {
  return needCommands;
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToPoint::visualization(const Visualizator& visualizator) const {
  for (size_t index = 1; index < path.size(); index++) {
    visualizator.line(path[index - 1].x, path[index - 1].y, path[index].x, path[index].y, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR