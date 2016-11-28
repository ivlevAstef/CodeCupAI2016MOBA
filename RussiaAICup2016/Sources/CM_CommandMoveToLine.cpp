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

bool CommandMoveToLine::check(const Wizard& self) {
  /// Чем больше хп, тем больше можно наглеть и идти вперед
  const auto offset = 5 * (80 * self.getRole().getAudacity() - ((100 * self.getLife()) / self.getMaxLife()));
  auto position = InfluenceMap::instance().getForeFront(line, float(offset));
  commandMoveToPoint = std::make_shared<CommandMoveToPoint>(pathFinder, position.x, position.y);

  return commandMoveToPoint->check(self);
}


void CommandMoveToLine::execute(const Wizard& self, Result& result) {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->execute(self, result);
  result.priority = MovePriorities::moveToLine(self, line);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToLine::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != commandMoveToPoint.get());
  commandMoveToPoint->visualization(self, visualizator);
}
#endif // ENABLE_VISUALIZATOR