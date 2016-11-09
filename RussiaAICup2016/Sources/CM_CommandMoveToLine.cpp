//
//File: CM_CommandMoveToLine.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToLine.h"
#include "CM_Move.h"
#include "E_World.h"

using namespace AICup;

CommandMoveToLine::CommandMoveToLine(model::LineType line): line(line) {

}

bool CommandMoveToLine::check(const model::Wizard& self, model::Move& move) {
  auto position = World::instance().linePosition(line);

  path = Move::path(Position(self.getX(), self.getY()), position, pathLength);

  return path.size() >= 2 && pathLength > 1;
}

void CommandMoveToLine::execute(const model::Wizard& self, model::Move& move) {
  const auto action = Move::move(self, path);

  move.setSpeed(action.speed);
  move.setStrafeSpeed(action.strafeSpeed);
  move.setTurn(action.turn);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToLine::visualization(const Visualizator& visualizator) const {
  for (size_t index = 1; index < path.size(); index++) {
    visualizator.line(path[index - 1].x, path[index - 1].y, path[index].x, path[index].y, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR