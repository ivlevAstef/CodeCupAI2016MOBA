//
//File: CM_CommandMoveToLine.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToLine.h"
#include "E_World.h"

using namespace AICup;

CommandMoveToLine::CommandMoveToLine(model::LaneType line): line(line) {

}

bool CommandMoveToLine::check(const model::Wizard& self, model::Move& move) {
  auto position = World::instance().linePosition(line);
  commandMoveToPoint = std::make_shared<CommandMoveToPoint>(position.x, position.y);

  return commandMoveToPoint->check(self, move);
}

void CommandMoveToLine::execute(const model::Wizard& self, model::Move& move) {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->execute(self, move);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToLine::visualization(const Visualizator& visualizator) const {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR