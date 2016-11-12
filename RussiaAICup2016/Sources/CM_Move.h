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
    /// ������ ���� ��������� �� ������ �����, �� ������� ����� ������ �� ������ ����� ������� �� ����� � �����
    std::vector<Position> path(const Position from, const Position to, double& length);


    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����
    MoveAction move(const model::CircularUnit& unit, const Position& pos, MoveStyle style = MOVE_WITH_ROTATE);

    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����, �������� ����������
    MoveAction move(const model::CircularUnit& unit, const Position& pos, const ObstaclesGroups& obstacles, MoveStyle style = MOVE_WITH_ROTATE);

    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����, ��������� ����
    MoveAction move(const model::CircularUnit& unit, const Path& path, MoveStyle style = MOVE_WITH_ROTATE);
    MoveAction move(const model::Wizard& unit, const Path& path, MoveStyle style = MOVE_WITH_ROTATE);

    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����, ��������� ���� � �������� �����������
    MoveAction move(const model::CircularUnit& unit, const Path& path, const ObstaclesGroups& obstacles, MoveStyle style = MOVE_WITH_ROTATE);
    MoveAction move(const model::Wizard& unit, const Path& path, const ObstaclesGroups& obstacles, MoveStyle style = MOVE_WITH_ROTATE);

  };

};