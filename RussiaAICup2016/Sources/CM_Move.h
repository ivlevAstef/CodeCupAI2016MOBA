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

  /// ������ ���� ��������� �� ������ �����, �� ������� ����� ������ �� ������ ����� ������� �� ����� � �����
  std::vector<Position> path(Position from, Position to);

  /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����
  MoveAction move(MovableUnit unit, Position to);

};