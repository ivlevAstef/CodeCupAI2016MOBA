//
//File: CM_CommandMoveToLine.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToLine.h"
#include "E_InfluenceMap.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "C_Extensions.h"

using namespace AICup;

CommandMoveToLine::CommandMoveToLine(model::LaneType line): line(line) {

}

bool CommandMoveToLine::check(const Wizard&) {
  return true;
}


void CommandMoveToLine::execute(const Wizard& self, Result& result) {
  const auto offset = -100 * self.getRole().getAudacity();
  toPoint = InfluenceMap::instance().getForeFront(line, float(offset));

  result.set(toPoint, self);
  result.turnStyle = TurnStyle::TURN;
  result.turnPriority = TurnPriority::moveToLine;
}

double CommandMoveToLine::priority(const Wizard& self) {
  return MovePriorities::moveToLine(self, line);
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToLine::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, toPoint.x, toPoint.y, 0x00ff00);
  }
}
#endif // ENABLE_VISUALIZATOR