//
//File: CM_Move.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "C_Vector2D.h"
#include "model\Wizard.h"
#include "E_Types.h"
#include <vector>

namespace AICup
{
  struct MoveAction {
    double speed;
    double strafeSpeed;
    double turn;
    bool isClosed;
  };

  enum MoveStyle {
    MOVE_WITH_ROTATE,
    MOVE_CONST_ANGLE,
    MOVE_WITH_BACKWARD_ROTATE
  };

  namespace Move
  {
#define SPEED_LIMIT_NOT_SET -100

    /// строит путь состоящий из набора точек, по которым можно пройти по прямым чтобы попасть из точки в точку
    std::vector<Position> path(const Position from, const Position to, double& length);

    /// Говорит куда и как надо двигаться, чтобы попасть из текущего состояния в некоторую точку
    MoveAction move(const model::CircularUnit& unit, const Position& pos, const double speedLimit, MoveStyle style);

    /// Говорит куда и как надо двигаться, чтобы попасть из текущего состояния в некоторую точку, учитывая преятствия
    MoveAction move(const model::CircularUnit& unit, const Position& pos, const ObstaclesGroups& obstacles, const double speedLimit, MoveStyle style);

    /// Говорит куда и как надо двигаться, чтобы попасть из текущего состояния в некоторую точку, используя путь
    MoveAction move(const model::CircularUnit& unit, const Path& path, const double speedLimit, MoveStyle style);
    MoveAction move(const model::Wizard& unit, const Path& path, const double speedLimit, MoveStyle style);

    /// Говорит куда и как надо двигаться, чтобы попасть из текущего состояния в некоторую точку, используя путь и учитывая препятствия
    MoveAction move(const model::CircularUnit& unit, const Path& path, const ObstaclesGroups& obstacles, const double speedLimit, MoveStyle style);
    MoveAction move(const model::Wizard& unit, const Path& path, const ObstaclesGroups& obstacles, const double speedLimit, MoveStyle style);

  };

};