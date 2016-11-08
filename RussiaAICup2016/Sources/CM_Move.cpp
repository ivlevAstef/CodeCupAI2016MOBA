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

MoveAction Move::move(const MovableUnit unit, const Position& to, bool rotate) {
  const auto dx = unit.x - to.x;
  const auto dy = unit.y - to.y;
  auto vec = Vector(dx, dy);
  auto rotatedVec = vec.rotated(unit.angle);
  rotatedVec = rotatedVec.normal() * Game::instance().model().getWizardForwardSpeed();

  if (rotate) {
    const auto vecAngle = vec.angle();
    const auto diff = -Extensions::angleDiff(vecAngle, unit.angle);
    return MoveAction{rotatedVec.x, rotatedVec.y, diff*0.1};
  }

  return MoveAction{rotatedVec.x, rotatedVec.y, 0};
}

MoveAction Move::move(const MovableUnit unit, const std::vector<Position>& path, bool rotate) {
  assert(path.size() >= 2);

  if ((path[1] - path[0]).length2() < 1 && path.size() >= 3) {
    return move(unit, path[2], rotate);
  }


  return move(unit, path[1], rotate);
}