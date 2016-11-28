//
//File: CM_CommandMoveToPoint.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToPoint.h"
#include "A_Move.h"
#include "E_World.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "C_Math.h"

using namespace AICup;

/// так как какие-нибудь алгоритмы аля follow могут выдать плохие координаты, то лучше их обрезать
CommandMoveToPoint::CommandMoveToPoint(Algorithm::PathFinder& finder, const double x, const double y, const TurnStyle style, const double speedLimit):
  MoveCommand(finder), point(MAX(0,MIN(x, World::size())), MAX(0, MIN(y, World::size()))), style(style), speedLimit(speedLimit) {

}

bool CommandMoveToPoint::check(const Wizard& self) {
  return (EX::pos(self) - point).length() > EX::maxSpeed(self);
}


void CommandMoveToPoint::execute(const Wizard& self, Result& result) {
  pathFinder.calculatePath(point, path);
  assert(nullptr != path);

  preEndPoint = path->calculateNearestCurvaturePoint(self.getVisionRange()/2);

  /// need remove obstacles
  auto obstacles = World::instance().obstacles(self, self.getVisionRange());
  result.treesForRemove = path->removeObstacles(obstacles);
  const auto obstaclesGroups = World::instance().createGroup(obstacles, self.getRadius());

  result.moveDirection = Algorithm::move(self, preEndPoint, obstaclesGroups, self.getVisionRange());
  result.turnStyle = style;
  result.priority = MovePriorities::moveToPoint(self, point);
  result.speedLimit = speedLimit;

  endPoint = path->getFrom() + result.moveDirection;
}

#ifdef ENABLE_VISUALIZATOR
void CommandMoveToPoint::visualization(const model::Wizard&, const Visualizator& visualizator) const {
  assert(nullptr != path);
  path->visualization(visualizator);

  if (Visualizator::POST == visualizator.getStyle()) {
    const auto& from = path->getFrom();

    visualizator.line(from.x, from.y, endPoint.x, endPoint.y, 0xff00ff);
    visualizator.line(from.x, from.y, preEndPoint.x, preEndPoint.y, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR