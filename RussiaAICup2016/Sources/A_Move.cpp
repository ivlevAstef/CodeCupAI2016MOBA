#include "A_Move.h"
#include "C_Math.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include <algorithm>

using namespace AICup;


/// ������� ��������� ������, � ������� ������������� ������ �������� �����.
const Obstacles* findNearestGroup(const Position& from, const double radius, const Position& to, const ObstaclesGroups& obstacles, double& minDistance) {
  const Obstacles* nearestGroup = nullptr;

  for (const auto& group : obstacles) {
    for (const auto& obstacle : group) {
      const auto obstaclePos = EX::pos(obstacle);
      const auto fullRadius = radius + obstacle.getRadius();

      /// ��������� ��������� � ������������, �� ������������ �������, ��� ��� ����������� ����� �� ������ ���� � �����
      if (Math::distanceToLine(obstaclePos, to, from) < fullRadius) {
        /// ����������� �� ����� ������
        if ((to - from).dot(obstaclePos - from) < 0) {
          continue;
        }

        /// ����������� ������ ����� ��������� ��������
        if ((to - from).length() < (obstaclePos - from).length() - fullRadius) {
          continue;
        }

        double distance = (from - obstaclePos).length() - fullRadius;
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
  const Position p2 = from + vec;
  for (const auto& obstacle : group) {
    const auto obstaclePos = EX::pos(obstacle);
    /// ���������� ������ ����
    if ((obstaclePos - checkPos).length2() < 0.1) {
      continue;
    }

    const double distance = Math::distanceToLine(obstaclePos, from, p2);
    if (vec.dot(obstaclePos - from) >=0 && distance <= obstacle.getRadius() + radius) {
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

        if (2 == resultIndex) {
          goto findGroupPartsAndReturnTangets_for_END; // �������� + ������
        }
      }
    }
  }
  findGroupPartsAndReturnTangets_for_END:;

  assert(2 == resultIndex || 0 == resultIndex);
  if (2 != resultIndex) {
    /// ��� ����������� ����� �� ������ - �������.
    return Vector();
  }

  resultIndex = (result[0].deviation < result[1].deviation) ? 0 : 1;
  return result[resultIndex].tangent * result[resultIndex].length;

  /*if (result[0].tangent.dot(to - from)

  resultIndex = 0; // ������ � ������� ��������
  return result[resultIndex].tangent * result[resultIndex].length;

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

  return result[resultIndex].tangent * result[resultIndex].length;*/
}

Vector Algorithm::move(const model::CircularUnit& unit, const Position& to, const ObstaclesGroups& obstacles, const double range) {
  const auto from = EX::pos(unit);

  Vector toIter = to;
  Vector tangent = to - from;
  bool found = false;

  ObstaclesGroups obstaclesGroups = obstacles;

  while (obstaclesGroups.size() > 0) {
    double minDistance = range;
    const Obstacles* nearestGroup = findNearestGroup(from, unit.getRadius(), toIter, obstaclesGroups, minDistance);
    if (nullptr == nearestGroup) { // ����������� �� ���� ���������� ���
      if (found) {
        tangent = tangent.normal() * minDistance;
      }
      break;
    }

    found = true;
    tangent = findGroupPartsAndReturnTangets(from, unit.getRadius(), to, *nearestGroup);

    /// ��������� � ���������
    if (tangent.length() < 1.0e-9) {
      return Vector();
    }

    toIter = from + ((tangent.length() < minDistance) ? tangent : tangent.normal() * minDistance);

    // ������� ������ � ������� ��� �����������
    const size_t groupIndex = nearestGroup - &obstaclesGroups[0];
    obstaclesGroups.erase(obstaclesGroups.begin() + groupIndex);
  }

  return tangent;
}
