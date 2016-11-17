//
//File: A_Move.h
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
  namespace Algorithm
  {
    /// ������ ���� ��������� �� ������ �����, �� ������� ����� ������ �� ������ ����� ������� �� ����� � �����
    std::vector<Position> path(const Position from, const Position to, double& length);

    /// ��������� ������ ��������, �������� �����������
    Vector move(const model::CircularUnit& unit, const Position& pos, const ObstaclesGroups& obstacles);

    ///  ��������� ������ ��������, �������� ����
    Vector move(const model::CircularUnit& unit, const Path& path);
    Vector move(const model::Wizard& unit, const Path& path);

    ///  ��������� ������ ��������, �������� ���� � �����������
    Vector move(const model::CircularUnit& unit, const Path& path, const ObstaclesGroups& obstacles);
    Vector move(const model::Wizard& unit, const Path& path, const ObstaclesGroups& obstacles);

  };

};