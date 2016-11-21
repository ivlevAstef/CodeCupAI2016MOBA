//
//File: CM_CommandMoveToPoint.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToPoint.h"
#include "A_Move.h"
#include "E_World.h"
#include "C_Extensions.h"
#include "A_PathFinder.h"

using namespace AICup;

CommandMoveToPoint::CommandMoveToPoint(const double x, const double y, const TurnStyle style, const double speedLimit):
  point(x, y), style(style), speedLimit(speedLimit) {
}

bool CommandMoveToPoint::check(const model::Wizard& self) {
  from = EX::pos(self);
  return (from - point).length() > 20;
}

int CommandMoveToPoint::priority(const model::Wizard& self) {
  return 0;
}

void CommandMoveToPoint::execute(const model::Wizard& self, Result& result) {
  path.setTo(point);
  preEndPoint = path.calculate(self, self.getVisionRange());

  const auto obstaclesGroups = World::instance().obstaclesGroup(self, self.getVisionRange());

  result.moveDirection = Algorithm::move(self, preEndPoint, obstaclesGroups, self.getVisionRange());
  result.turnStyle = style;
  result.priority = priority(self);
  result.speedLimit = speedLimit;

  endPoint = from + result.moveDirection;
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToPoint::visualization(const Visualizator& visualizator) const {
  path.visualization(visualizator);

  visualizator.line(from.x, from.y, preEndPoint.x, preEndPoint.y, 0xff0000);
  visualizator.line(from.x, from.y, endPoint.x, endPoint.y, 0xff00ff);
}
#endif // ENABLE_VISUALIZATOR