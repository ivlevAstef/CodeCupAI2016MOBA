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

    /// ������ ���� ��������� �� ������ �����, �� ������� ����� ������ �� ������ ����� ������� �� ����� � �����
    std::vector<Position> path(const Position from, const Position to, double& length);

    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����
    MoveAction move(const model::CircularUnit& unit, const Position& pos, const double speedLimit, MoveStyle style);

    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����, �������� ����������
    MoveAction move(const model::CircularUnit& unit, const Position& pos, const ObstaclesGroups& obstacles, const double speedLimit, MoveStyle style);

    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����, ��������� ����
    MoveAction move(const model::CircularUnit& unit, const Path& path, const double speedLimit, MoveStyle style);
    MoveAction move(const model::Wizard& unit, const Path& path, const double speedLimit, MoveStyle style);

    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����, ��������� ���� � �������� �����������
    MoveAction move(const model::CircularUnit& unit, const Path& path, const ObstaclesGroups& obstacles, const double speedLimit, MoveStyle style);
    MoveAction move(const model::Wizard& unit, const Path& path, const ObstaclesGroups& obstacles, const double speedLimit, MoveStyle style);

  };

};