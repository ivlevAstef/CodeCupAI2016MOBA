//
//File: CM_CommandMoveToLine.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToLine.h"
#include "CM_CommandMoveToPoint.h"
#include "E_InfluenceMap.h"
#include "CM_MovePriorities.h"

using namespace AICup;

CommandMoveToLine::CommandMoveToLine(Algorithm::PathFinder& finder, model::LaneType line):
  MoveCommand(finder), line(line) {

}

bool CommandMoveToLine::check(const model::Wizard& self) {
  /// Чем больше хп, тем больше можно наглеть и идти вперед
  auto position = InfluenceMap::instance().getForeFront(line, float(5 * (80 - self.getLife())));
  commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pathFinder, position.x, position.y);

  return commandMoveToPoint->check(self);
}


void CommandMoveToLine::execute(const model::Wizard& self, Result& result) {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->execute(self, result);
  result.priority = MovePriorities::moveToLine(self, line);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToLine::visualization(const Visualizator& visualizator) const {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->visualization(visualizator);
}
#endif // ENABLE_VISUALIZATOR