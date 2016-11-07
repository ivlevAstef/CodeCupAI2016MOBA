//
//File: CM_Move.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "C_Vector2D.h"
#include <vector>

namespace AICup
{
  struct MoveAction {
    double speed;
    double strafeSpeed;
    double turn;
  };

  struct MovableUnit {
    double x;
    double y;
    double turn;
  };

  /// строит путь состоящий из набора точек, по которым можно пройти по прямым чтобы попасть из точки в точку
  std::vector<Position> path(Position from, Position to);

  /// Говорит куда и как надо двигаться, чтобы попасть из текущего состояния в некоторую точку
  MoveAction move(MovableUnit unit, Position to);

};