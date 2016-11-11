//
//File: CM_Move.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "CM_Move.h"
#include "E_Graph.h"
#include "C_Math.h"
#include "E_Game.h"

using namespace AICup;

std::vector<Position> Move::path(const Position from, const Position to, double& length) {
  return Graph::instance().path(from, to, length);
}

MoveAction Move::move(const model::CircularUnit& unit, const Position& to, MoveStyle style) {
  const auto dx = to.x - unit.getX();
  const auto dy = to.y - unit.getY();
  auto speedVec = Vector(dx, -dy).normalize().rotated(unit.getAngle());
  speedVec.y *= -1;
  speedVec *= Game::instance().model().getWizardForwardSpeed();

  if (MOVE_WITH_ROTATE == style) {
    const auto vecAngle = Vector(dx, dy).angle();
    const auto diff = Math::angleDiff(vecAngle, unit.getAngle());
    return MoveAction{speedVec.x, speedVec.y, diff};
  } else if (MOVE_WITH_BACKWARD_ROTATE == style) {
    const auto vecAngle = Vector(dx, dy).angle() + AICUP_PI;
    const auto diff = Math::angleDiff(vecAngle, unit.getAngle());
    return MoveAction{speedVec.x, speedVec.y, diff};
  } else if (MOVE_CONST_ANGLE == style) {
    return MoveAction{speedVec.x, speedVec.y, 0};
  }

  assert(false && "incorrect move style");
  return MoveAction{0, 0, 0};
}

MoveAction Move::move(const model::CircularUnit& unit, const Position& pos, const ObstaclesGroups& obstacles, MoveStyle style) {
  // � ���� ����� ��������� �������������� ������������� (�� ������ ��� ������)

  // ����������� ����� ��������� = �������������

  // �������� ������ �������� �� ����� ����������������, � ���� � �������� �������������, � ���������� �� ������� �� ������������ ����� ���������, ����� ����� �������
  // ������ �� ���� ������ � ������, ����� ��� �����, ������� �� ������� ����� ������������ ���������� �� �������� ����������� (� ������ �������)
  // �� ���� ����� ������� �� � �������� ���� ���������� �������� �� ������ ���������.
  // ��������� ����������� ��� ���� �����������: ��� + ����� � � ��������
  // ��� ����������� ���������� ����� �������� ��������, � ������������ ����� ��������� = ���������� �� �����
  // ��������� ��� ������ �������

  // ���� ��� �������� ���� ��������, ��������� ���, ��� ���������� ����� ���� ������ ������� ����, ������ ��� � �����.

  return move(unit, pos, style);
}

Position simplePathToPosition(const Path& path) {
  assert(path.size() >= 2);

  if ((path[1] - path[0]).length() < 8/*TODO: magic constant*/ && path.size() >= 3) {
    return path[2];
  }

  return path[1];
}

Position adaptivePathToPosition(const Path& path, const model::Wizard& unit) {
  assert(path.size() >= 1);

  const auto unitPos = Position(unit.getX(), unit.getY());
  const auto radius = unit.getVisionRange();

  const auto& lastPos = path[path.size() - 1];
  /// ���� ��������� ����� ���������� � ������, �� ��������� � ���
  if ((unitPos - lastPos).length2() < radius*radius) {
    return lastPos;
  }

  for (size_t i = path.size()-1; i > 0; i--) { // ������ ����� ���������� ��-�� (i-1) � ���������
    const auto intersectPoints = Math::intersectLineWithCircle(unitPos, radius, path[i - 1], path[i]);

    if (1 == intersectPoints.size()) {
      return intersectPoints[0];
    }

    if (2 == intersectPoints.size()) {
      double len1 = (path[i] - intersectPoints[0]).length2();
      double len2 = (path[i] - intersectPoints[1]).length2();
      if (len1 < len2) {
        return intersectPoints[0];
      }
      return intersectPoints[1];
    }
  }

  assert(false && "Can not found intersects points for path... maybe path not belongs this Wizard?");
  return lastPos;
}


MoveAction Move::move(const model::CircularUnit& unit, const Path& path, MoveStyle style) {
  const auto pos = simplePathToPosition(path);
  return move(unit, pos, style);
}

MoveAction Move::move(const model::Wizard& unit, const Path& path, MoveStyle style) {
  const auto pos = simplePathToPosition(path);
  /// ��� ��� ���������� ���� ��������� ��� ����� ��� ����� ������� ����� �� ���������,
  /// �� ����� ��������� ��� ��� ����� �����������, ��� ��� ������ ����� �����, � �� � ��� ��������
  /// const auto pos = adaptivePathToPosition(path, unit);

  return move(unit, pos, style);
}

MoveAction Move::move(const model::CircularUnit& unit, const Path& path, const ObstaclesGroups& obstacles, MoveStyle style) {
  const auto pos = simplePathToPosition(path);
  return move(unit, pos, obstacles, style);
}

MoveAction Move::move(const model::Wizard& unit, const Path& path, const ObstaclesGroups& obstacles, MoveStyle style) {
  const auto pos = adaptivePathToPosition(path, unit);
  return move(unit, pos, obstacles, style);
}