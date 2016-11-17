//
//File: CM_CommandMoveToLine.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToLine.h"
#include "CM_CommandMoveToPoint.h"
#include "E_World.h"

using namespace AICup;

CommandMoveToLine::CommandMoveToLine(model::LaneType line): line(line) {

}

bool CommandMoveToLine::check(const model::Wizard& self) {
  auto position = World::instance().linePosition(line);
  commandMoveToPoint = std::make_shared<CommandMoveToPoint>(position.x, position.y);

  return commandMoveToPoint->check(self);
}

int CommandMoveToLine::priority(const model::Wizard& self) {
  return 0;
}


void CommandMoveToLine::execute(const model::Wizard& self, Result& result) {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->execute(self, result);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToLine::visualization(const Visualizator& visualizator) const {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR