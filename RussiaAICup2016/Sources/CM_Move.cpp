//
//File: CM_Move.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "CM_Move.h"
#include "E_Graph.h"
#include "C_Extensions.h"
#include "E_Game.h"

using namespace AICup;

std::vector<Position> Move::path(const Position from, const Position to, double& length) {
  return Graph::instance().path(from, to, length);
}

MoveAction Move::move(const model::CircularUnit& unit, const Position& to, bool rotate) {
  const auto dx = to.x - unit.getX();
  const auto dy = to.y - unit.getY();
  auto speedVec = Vector(dx, -dy).normalize().rotated(unit.getAngle());
  speedVec.y *= -1;
  speedVec *= Game::instance().model().getWizardForwardSpeed();

  if (rotate) {
    const auto vecAngle = Vector(dx, dy).angle();
    const auto diff = Extensions::angleDiff(vecAngle, unit.getAngle());
    return MoveAction{speedVec.x, speedVec.y, diff};
  }

  return MoveAction{speedVec.x, speedVec.y, 0};
}

MoveAction Move::move(const model::CircularUnit& unit, const std::vector<Position>& path, bool rotate) {
  assert(path.size() >= 2);

  if ((path[1] - path[0]).length() < unit.getRadius()*0.25 && path.size() >= 3) {
    return move(unit, path[2], rotate);
  }


  return move(unit, path[1], rotate);
}