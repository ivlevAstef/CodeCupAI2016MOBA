//
//File: CM_CommandMoveToPoint.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToPoint.h"
#include "A_Move.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"

using namespace AICup;

CommandMoveToPoint::CommandMoveToPoint(const double x, const double y):
  point(x,y) {
}

bool CommandMoveToPoint::check(const Wizard& self) {
  return (EX::pos(self) - point).length() > 0.1;
}


void CommandMoveToPoint::execute(const Wizard& self, Result& result) {
  result.set(point, self);

  result.turnPriority = TurnPriority::moveToPoint;
  result.priority = MovePriorities::moveToPoint(self, point);
}


#ifdef ENABLE_VISUALIZATOR
void CommandMoveToPoint::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, point.x, point.y, 0x00ff00);
  }
}
#endif // ENABLE_VISUALIZATOR