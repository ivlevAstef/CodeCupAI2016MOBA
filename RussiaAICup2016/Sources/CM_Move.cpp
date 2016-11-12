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
    return MoveAction{speedVec.x, speedVec.y, diff, false};
  } else if (MOVE_WITH_BACKWARD_ROTATE == style) {
    const auto vecAngle = Vector(dx, dy).angle() + AICUP_PI;
    const auto diff = Math::angleDiff(vecAngle, unit.getAngle());
    return MoveAction{speedVec.x, speedVec.y, diff, false};
  } else if (MOVE_CONST_ANGLE == style) {
    return MoveAction{speedVec.x, speedVec.y, 0, false};
  }

  assert(false && "incorrect move style");
  return MoveAction{0, 0, 0, false};
}

/// ������� ��������� ������, � ������� ������������� ������ �������� �����.
const Obstacles* findNearestGroup(const Position& from, const double radius, const Position& to, const ObstaclesGroups& obstacles) {
  double minDistance = DBL_MAX;
  const Obstacles* nearestGroup = nullptr;

  for (const auto& group : obstacles) {
    for (const auto& obstacle : group) {
      const auto obstaclePos = Position(obstacle.getX(), obstacle.getY());

      /// ��������� ��������� � ������������, �� ������������ �������, ��� ��� ����������� ����� �� ������ ���� � �����
      if (Math::distanceToLine(obstaclePos, to, from) < obstacle.getRadius() + radius) {
        /// ����������� �� ����� ������
        if ((to - from).dot(obstaclePos - from) < 0) {
          continue;
        }

        /// ����������� ������ ����� ��������� ��������
        if ((from - to).dot(obstaclePos - to) < 0) {
          continue;
        }

        double distance = (from - obstaclePos).length();
        if (distance < minDistance) {
          minDistance = distance;
          nearestGroup = &group;
        }
      }
    }
  }

  return nearestGroup;
}


const bool checkIsClosed(const model::CircularUnit& obstacle1, const model::CircularUnit& obstacle2, const double radius) {
  return false; //TODO: ��������, ���� ���������, ��� ��� ���� ������ ���� ����� �������� � �� �������
  /*const double dx = obstacle2.getX() - obstacle1.getX();
  const double dy = obstacle2.getY() - obstacle1.getY();

  return sqrt(dx*dx + dy*dy) < obstacle1.getRadius() + obstacle2.getRadius() + 2 * radius;*/
}

/// ���� ���� ������ - ����� � ������ �� ������� ������ ��������� �� �������� ����������. ����� ����� ��� - � ������ ������� ��������
/// ����� ���� ��� �������� �������, ������� ������� ��� ������� ������� ����� ����������� ����������.
/// ��������� ����� ��� ���� �� ��������� � ������ ��������� - ��������� ������������ ��� �������� ����������
const model::CircularUnit* findGroupPartsAndSelect(const Position& from, const double radius, const Position& to, const Obstacles& group, /*inout*/ bool& isClosed) {
  assert(!group.empty());

  const auto delta = (to - from).normal();

  static const size_t LeftMax = 0;
  static const size_t LeftMin = 1;
  static const size_t RightMax = 2;
  static const size_t RightMin = 3;
  double deviations[4] = {
    -1, // left max
     3, // left min
    -1, // right max
     3  // right min
  };

  const model::CircularUnit* obstacles[4] = {
    nullptr, // left max
    nullptr, // left min
    nullptr, // right max
    nullptr, // right min
  };

  for (const auto& obstacle : group) {
    const auto obstaclePos = Position(obstacle.getX(), obstacle.getY());
    const auto obstacleVector = obstaclePos - from;
    /// ��������� ������� ����������� ������� ���, ��� ��� ��������� �� ������ ��������
    const double simulationTangentaFirstStep = 1 - (obstacle.getRadius() + radius) / obstacleVector.length();
    const double perLen = obstacle.getRadius() + radius + 1 / MAX(0.001, simulationTangentaFirstStep);
    const auto obstacleDeltaPer = obstacleVector.normal().perpendicular() * perLen;

    const auto obstacleDelta1 = (obstacleVector + obstacleDeltaPer).normal();
    const auto obstacleDelta2 = (obstacleVector - obstacleDeltaPer).normal();

    for (const auto& obstacleDelta : {obstacleDelta1, obstacleDelta2}) {
      /// ���������� �� ��������� ��������, ��� ������ ��� ������ ����������, �������� �� 0 �� 2
      double deviation = 1 - delta.dot(obstacleDelta);

      size_t tIndex = 0;
      /// ��������� ����������� ����������
      if (delta.cross(obstacleDelta) < 0) {
        tIndex = 0; // left
      } else {
        tIndex = 2; // right
      }

      /// max deviation
      if (deviations[tIndex] < deviation) {
        deviations[tIndex] = deviation;
        obstacles[tIndex] = &obstacle;
      }

      /// min deviation
      if (deviations[tIndex + 1] > deviation) {
        deviations[tIndex + 1] = deviation;
        obstacles[tIndex + 1] = &obstacle;
      }
    }
  }

  /// ���� ����� ����, ������ ������ ����� ��������� �������
  if (nullptr == obstacles[LeftMax]) {
    return obstacles[RightMin];
  }

  /// ���� ������ ����, ������ ������ ����� ��������� �������
  if (nullptr == obstacles[RightMax]) {
    return obstacles[LeftMin];
  }

  /// ���� ���� ���, ��������� ��� ��� �� ������� ����
  isClosed = checkIsClosed(*obstacles[LeftMax], *obstacles[RightMax], radius);

  /// ���� ���� ��� ������� ����� �� ��� ���������
  if (deviations[LeftMax] < deviations[RightMax]) {
    return obstacles[LeftMax];
  } else {
    return obstacles[RightMax];
  }

  /* ������� ���� ���������� ���
  /// � ����� ������� ��� ���������� - ������ ��� ��������� ������
  if (nullptr == obstacles[LeftMax]) {
    obstacles[LeftMax] = obstacles[RightMin];
  }

  /// � ������ ������� ��� ���������� - ������ ��� ��������� �����
  if (nullptr == obstacles[RightMax]) {
    obstacles[RightMax] = obstacles[LeftMin];
  }

  return {obstacles[LeftMax], obstacles[RightMax]};
  */
}

const model::CircularUnit* findNeighbor(const model::CircularUnit& obstacle, const Obstacles& group) {
  const auto obstaclePos = Position(obstacle.getX(), obstacle.getY());

  const model::CircularUnit* neighbor = nullptr;
  double minDistance = DBL_MAX;
  for (const auto& subObstacle : group) {
    double distance = (obstaclePos - Position(subObstacle.getX(), subObstacle.getY())).length2();

    if (obstacle.getId() != subObstacle.getId() && distance < minDistance) {
      minDistance = distance;
      neighbor = &subObstacle;
    }
  }

  if (nullptr == neighbor) {
    return &obstacle;
  }

  return neighbor;
}

/// ��������� ������ �� v1,v2 ������� �������� ����� ������ �����������. ���� v1,v2 ��� ����������� � �����������
const Vector bestVector(const Position& from, const Position& obstaclePos, const Position& neighbor, const Vector& v1, const Vector& v2) {

  Position delta;
  /// ���� ����������� ��� ������������ ����
  if ((obstaclePos - neighbor).length2() < 1.0e-1) {
    delta = (obstaclePos - from).normal();
  } else {
    delta = (obstaclePos - neighbor).normal();
  }

  /// ��� ��� ��������, ��� �� ������, ��� ������ ����������
  if (delta.dot(v1.normal()) < delta.dot(v2.normal())) {
    return v2;
  }
  return v1;
}

/// ������������� ����������� �������� � ������� ������ ���� ��� �� ��� (��� ��� ����������� ����� ����� ����� ����
void setVectorDirection(const Position& from, const Position& obstaclePos, Vector& v1, Vector& v2) {
  const auto delta = obstaclePos - from;

  if (v1.dot(delta) < 0) {
    v1.set(-v1);
  }

  if (v2.dot(delta) < 0) {
    v2.set(-v2);
  }
}

class MyCircularUnit: public model::CircularUnit {
public:
  MyCircularUnit(const CircularUnit& unit, int ticks): model::CircularUnit(
    unit.getId(),
    unit.getX() + unit.getSpeedX() * ticks,
    unit.getY() + unit.getSpeedY() * ticks,
    unit.getSpeedX(), unit.getSpeedY(),
    unit.getAngle(),
    unit.getFaction(),
    unit.getRadius()*1.1
    )
  {
  }
};

void moveObstacles(const ObstaclesGroups& obstaclesGroups, ObstaclesGroups& result, int ticks = 0) {
  result.reserve(obstaclesGroups.size());


  for (const auto& group : obstaclesGroups) {
    result.push_back({ });
    Obstacles& resultGroup = result[result.size() - 1];
    resultGroup.reserve(group.size());

    for (const auto& obstacle : group) {
      resultGroup.push_back(MyCircularUnit(obstacle, ticks));
    }
  }
}

MoveAction Move::move(const model::CircularUnit& unit, const Position& to, const ObstaclesGroups& obstacles, MoveStyle style) {
  const auto from = Position(unit.getX(), unit.getY());

  Position iterTo = to;

  ObstaclesGroups obstaclesGroups;
  moveObstacles(obstacles, obstaclesGroups, 2);

  while (obstaclesGroups.size() > 0) {
    const Obstacles* nearestGroup = findNearestGroup(from, unit.getRadius(), iterTo, obstaclesGroups);
    if (nullptr == nearestGroup) { // ����������� �� ���� ���������� ���
      break;
    }

    bool isClosed = false;
    const auto obstacle = findGroupPartsAndSelect(from, unit.getRadius(), to, *nearestGroup, isClosed);
    assert(nullptr != obstacle);
    /// ��������� � ���������
    if (isClosed) {
      return MoveAction{0,0,0, true};
    }

    const auto obstaclePos = Position(obstacle->getX(), obstacle->getY());

    auto neighbor = findNeighbor(*obstacle, *nearestGroup);
    assert(nullptr != neighbor);
    const auto neighborPos = Position(neighbor->getX(), neighbor->getY());

    const auto moveVectors = Math::tangetsForTwoCircle(from, unit.getRadius(), obstaclePos, obstacle->getRadius());
    assert(2 == moveVectors.size());
    auto mVector1 = moveVectors[0];
    auto mVector2 = moveVectors[1];
    setVectorDirection(from, obstaclePos, mVector1, mVector2);

    const auto moveVector = bestVector(from, obstaclePos, neighborPos, mVector1, mVector2);

    iterTo = from + moveVector.normal() * (from - obstaclePos).length();

    const size_t groupIndex = (nearestGroup - &obstaclesGroups[0]) / sizeof(Obstacles);
    obstaclesGroups.erase(obstaclesGroups.begin() + groupIndex);
  };


  return move(unit, iterTo, style);
}

Position simplePathToPosition(const Path& path) {
  assert(path.size() >= 2);

  if ((path[1] - path[0]).length() < 35/*TODO: magic constant*/ && path.size() >= 3) {
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
    const auto intersectPoints = Math::intersectSegmentWithCircle(unitPos, radius, path[i - 1], path[i]);

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