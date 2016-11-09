//
//File: CM_Move.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "C_Vector2D.h"
#include "model\CircularUnit.h"
#include <vector>

namespace AICup
{
  struct MoveAction {
    double speed;
    double strafeSpeed;
    double turn;
  };

  namespace Move
  {
    /// ������ ���� ��������� �� ������ �����, �� ������� ����� ������ �� ������ ����� ������� �� ����� � �����
    std::vector<Position> path(const Position from, const Position to, double& length);


    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����
    MoveAction move(const model::CircularUnit& unit, const Position& pos, bool rotate = true);

    /// ������� ���� � ��� ���� ���������, ����� ������� �� �������� ��������� � ��������� �����, ��������� ����
    MoveAction move(const model::CircularUnit& unit, const std::vector<Position>& path, bool rotate = true);

  };

};