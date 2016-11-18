#include "A_Move.h"
#include "E_Graph.h"
#include "C_Math.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include <algorithm>

using namespace AICup;

std::vector<Position> Algorithm::path(const Position from, const Position to, double& length) {
  return Graph::instance().path(from, to, length);
}


/// ���� ������ ������� �� ������� ����� ��� �����, ��� ��� �� ����� ���� �������� ����� �� �� ������.
/// ������� ��� ������� ���������� ������, ����� �� �� ������ �� ������� �����
const Position fitToMapRect(const model::CircularUnit& unit, const Position& toBase) {
  static const Vector axisOX = Position(1, 0);
  static const Vector axisOY = Position(0, 1);

  const double padding = unit.getRadius();
  const double size = 4000;

  const auto vec = toBase - Position(unit.getX(), unit.getY());
  double vecLength = vec.length();

  Position result(
    MAX(padding, MIN(toBase.x, size - padding)),
    MAX(padding, MIN(toBase.y, size - padding))
    );

  vecLength -= (result - Position(unit.getX(), unit.getY())).length();

  bool needXOverride = toBase.x < padding || toBase.x > size - padding;
  bool needYOverride = toBase.y < padding || toBase.y > size - padding;

  /// ���� �� �������� ���� � ����, �� ��� �����
  /// ���� ����� �������, ��� ��� ������
  if (needXOverride && needYOverride) {
    return result;
  }

  /// ����� ���������� ������ �� ��� - ������ ���� ��� �����
  /// ��������� �� ��� � ������ ������ �� ����� ������� ������� - ����� ������� ������

  if (needXOverride) {
    result += (axisOY * vec.dot(axisOY)).normal() * vecLength;
  }

  if (needYOverride) {
    result += (axisOX * vec.dot(axisOX)).normal() * vecLength;
  }


  return result;
}
//TODO: ������� ��� ������, ���� �������� �� ������ �������� �� ����, ������ �� ������ �������� ����
Vector calcVector(const model::CircularUnit& unit, const Position& to) {
  return fitToMapRect(unit, to) - EX::pos(unit);
}

/// ������� ��������� ������, � ������� ������������� ������ �������� �����.
const Obstacles* findNearestGroup(const Position& from, const double radius, const Position& to, const ObstaclesGroups& obstacles) {
  double minDistance = 100000;
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


/// ������������� ����������� �������� � ������� ������ ���� ��� �� ��� (��� ��� ����������� ����� ����� ����� ����
void setTangetsDirection(const Position& from, const Position& obstaclePos, std::vector<Vector>& tangets) {
  assert(2 == tangets.size());
  const auto delta = (obstaclePos - from).normal();

  /// �� ����, ����� �������� ������ �� ���������� 90 ��������
  if (tangets[0].dot(delta) < -0.001) {
    tangets[0].set(-tangets[0]);
  }

  if (tangets[1].dot(delta) < -0.001) {
    tangets[1].set(-tangets[1]);
  }
}


bool isIntersectVectorWithGroup(const Position& from, const Vector& vec, const Position& checkPos, const double radius, const Obstacles& group) {
  const Position p2 = from + vec * 10000;
  for (const auto& obstacle : group) {
    const auto obstaclePos = Position(obstacle.getX(), obstacle.getY());
    /// ���������� ������ ����
    if ((obstaclePos - checkPos).length2() < 1) {
      continue;
    }

    const double distance = Math::distanceToSegment(obstaclePos, from, p2);
    if (distance < obstacle.getRadius() + radius) {
      return true;
    }
  }

  return false;
}


struct ObstacleData {
  double deviation;
  Position pos;
  double length;
  Vector tangent;

};
/// ���� ���� ������ - ����� � ������ �� ������� ������ ��������� �� �������� ����������. ����� ����� ��� - � ������ ������� ��������
/// ����� ���� ��� �������� �������, ������� ������� ��� ������� ������� ����� ����������� ����������, � ������ ����������� ������ �����
/// ��������� ����� ��� ���� �� ��������� � ������ ��������� - ��������� ������������ ��� �������� ����������
Vector findGroupPartsAndReturnTangets(const Position& from, const double radius, const Position& to, const Obstacles& group) {
  assert(!group.empty());

  const auto delta = (to - from).normal();

  ObstacleData result[2] = {
    {999, Position(), 9999, Vector()},
    {999, Position(), 9999, Vector()}
  };
  double maxRadius = 0;
  int resultIndex = 0;


  for (const auto& obstacle : group) {
    const auto obstaclePos = EX::pos(obstacle);
    /// ��������� ��������������� ������� �����������
    auto obstacleTangets = Math::tangetsForTwoCircle(from, radius, obstaclePos, obstacle.getRadius());
    assert(2 == obstacleTangets.size());
    setTangetsDirection(from, obstaclePos, obstacleTangets);

    for (const auto& tangent : obstacleTangets) {
      if (!isIntersectVectorWithGroup(from, tangent, obstaclePos, radius, group)) {
        /// ���������� �� ��������� ��������, ��� ������ ��� ������ ����������, �������� �� 0 �� 2
        double deviation = 1 - delta.dot(tangent);
        double length = abs((obstaclePos - from).dot(tangent));

        assert(resultIndex < 2); // ������ ������ ���������� 2
        result[resultIndex++] = {deviation, obstaclePos,  MAX(1, length), tangent};
        maxRadius = MAX(maxRadius, obstacle.getRadius());
      }
    }
  }

  if (0 == resultIndex) {
    /// ��� ����������� ����� �� ������ - �������.
    return Vector();
  }
  assert(2 == resultIndex);

  int obstacleCount[2] = {0, 0};
  for (const auto& obstacle : group) {
    Vector obstacleVec = EX::pos(obstacle) - from;

    int sign0 = SIGN(delta.cross(obstacleVec));
    int sign1 = SIGN(result[0].tangent.cross(obstacleVec));
    //int sing2 = SIGN(result[1].tangent.cross(obstacleVec));
    if (sign1 == sign0) {
      obstacleCount[1]++;
    } else {
      obstacleCount[0]++;
    }
  }

  double distance = (result[0].pos - result[1].pos).length();
  if (obstacleCount[0] == obstacleCount[1] || Math::distanceToLine(from, result[0].pos, result[1].pos) > distance) {
    resultIndex = (result[0].deviation < result[1].deviation) ? 0 : 1;
  } else {
    resultIndex = (obstacleCount[0] < obstacleCount[1]) ? 0 : 1;
  }

  return result[resultIndex].tangent * result[resultIndex].length;
}

Vector Algorithm::move(const model::CircularUnit& unit, const Position& to, const ObstaclesGroups& obstacles) {
  const auto from = Position(unit.getX(), unit.getY());
  const auto minLength = EX::maxSpeed(unit);

  Vector tangent = to - from;

  ObstaclesGroups obstaclesGroups = obstacles;

  while (obstaclesGroups.size() > 0) {
    const Obstacles* nearestGroup = findNearestGroup(from, unit.getRadius(), from + tangent, obstaclesGroups);
    if (nullptr == nearestGroup) { // ����������� �� ���� ���������� ���
      break;
    }

    tangent = findGroupPartsAndReturnTangets(from, unit.getRadius(), to, *nearestGroup);
    double length = tangent.length();
    /// ��������� � ���������
    if (length< 1.0e-9) {
      return Vector();
    } else if (length < minLength) {
      tangent = tangent.normal() * minLength;
      break;
    }

    // ������� ������ � ������� ��� �����������
    const size_t groupIndex = (nearestGroup - &obstaclesGroups[0]) / sizeof(Obstacles);
    obstaclesGroups.erase(obstaclesGroups.begin() + groupIndex);
  };


  return calcVector(unit, from + tangent);
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
  if ((unitPos - lastPos).length2() - 1/*� �� ������ �� ������� ����� ���������*/ <= radius*radius) {
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


Vector Algorithm::move(const model::CircularUnit& unit, const Path& path) {
  const auto pos = simplePathToPosition(path);
  return calcVector(unit, pos);
}

Vector Algorithm::move(const model::Wizard& unit, const Path& path) {
  const auto pos = simplePathToPosition(path);
  /// ��� ��� ���������� ���� ��������� ��� ����� ��� ����� ������� ����� �� ���������,
  /// �� ����� ��������� ��� ��� ����� �����������, ��� ��� ������ ����� �����, � �� � ��� ��������
  /// const auto pos = adaptivePathToPosition(path, unit);

  return calcVector(unit, pos);
}

Vector Algorithm::move(const model::CircularUnit& unit, const Path& path, const ObstaclesGroups& obstacles) {
  const auto pos = simplePathToPosition(path);
  return move(unit, pos, obstacles);
}

Vector Algorithm::move(const model::Wizard& unit, const Path& path, const ObstaclesGroups& obstacles) {
  const auto pos = adaptivePathToPosition(path, unit);
  return move(unit, pos, obstacles);
}