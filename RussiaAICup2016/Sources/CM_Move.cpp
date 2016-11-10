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

MoveAction Move::move(const model::CircularUnit& unit, const Position& to, MoveStyle style) {
  const auto dx = to.x - unit.getX();
  const auto dy = to.y - unit.getY();
  auto speedVec = Vector(dx, -dy).normalize().rotated(unit.getAngle());
  speedVec.y *= -1;
  speedVec *= Game::instance().model().getWizardForwardSpeed();

  if (MOVE_WITH_ROTATE == style) {
    const auto vecAngle = Vector(dx, dy).angle();
    const auto diff = Extensions::angleDiff(vecAngle, unit.getAngle());
    return MoveAction{speedVec.x, speedVec.y, diff};
  } else if (MOVE_WITH_BACKWARD_ROTATE == style) {
    const auto vecAngle = Vector(dx, dy).angle() + AICUP_PI;
    const auto diff = Extensions::angleDiff(vecAngle, unit.getAngle());
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
}

const Position& pathToPosition(const Path& path) {
  assert(path.size() >= 2);

  if ((path[1] - path[0]).length() < 8/*TODO: magic constant*/ && path.size() >= 3) {
    return path[2];
  }

  return path[1];
}

MoveAction Move::move(const model::CircularUnit& unit, const Path& path, MoveStyle style) {
  return move(unit, pathToPosition(path), style);
}

MoveAction Move::move(const model::CircularUnit& unit, const Path& path, const ObstaclesGroups& obstacles, MoveStyle style) {
  return move(unit, pathToPosition(path), obstacles, style);
}