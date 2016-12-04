#include "A_Move.h"
#include "C_Math.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include <algorithm>

using namespace AICup;

Vector Algorithm::maxSpeed(const model::Wizard& wizard, const double wizardAngle, const Vector& direction) {
  Vector speed = Vector(direction.x, -direction.y).normal().rotated(wizardAngle);
  speed.y *= -1;

  const double maxXSpeed = (speed.x > 0) ? EX::maxSpeed(wizard) : EX::maxBackwardSpeed(wizard);
  const double maxYSpeed = EX::maxStrafeSpeed(wizard);

  speed.x *= maxXSpeed;
  speed.y *= maxYSpeed;

  double factor = sqrt(((speed.x*speed.x) / (maxXSpeed*maxXSpeed)) + ((speed.y*speed.y) / (maxYSpeed*maxYSpeed)));

  if (factor > 1) {
    speed.x /= factor;
    speed.y /= factor;
  }

  return speed;
}

Position Algorithm::offsetPointByPath(const Position& foreFront, float offset, const std::vector<Position>& line) {
  if (offset < 0) {
    std::vector<Position> reverseLine = line;
    std::reverse(reverseLine.begin(), reverseLine.end());
    return offsetPointByPath(foreFront, -offset, reverseLine);
  }

  size_t index = 0;
  for (index = 1; index < line.size(); index++) {
    if (Math::distanceToLine(foreFront, line[index - 1], line[index]) < 1.0e-3) {
      break;
    }
  }

  auto iterPos = foreFront;
  while (offset > 0 && index < line.size()) {
    //const auto& p1 = line[index - 1];
    const auto& p2 = line[index];
    const float length = float((p2 - iterPos).length());
    if (length < offset) {
      offset -= length;
      index++;
      iterPos = p2;
      continue;
    } else {
      iterPos = iterPos + ((p2 - iterPos) * (offset / length));
      break;
    }
  }

  return iterPos;
}


/// ������� ��������� ������, � ������� ������������� ������ �������� �����.
const Obstacles* findNearestGroup(const Position& from, const double radius, const Position& to, const ObstaclesGroups& obstacles, double& minDistance) {
  const Obstacles* nearestGroup = nullptr;

  for (const auto& group : obstacles) {
    for (const auto& obstacle : group) {
      const auto obstaclePos = EX::pos(*obstacle);
      const auto fullRadius = radius + obstacle->getRadius();

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
    const auto obstaclePos = EX::pos(*obstacle);
    /// ���������� ������ ����
    if ((obstaclePos - checkPos).length2() < 0.1) {
      continue;
    }

    const double distance = Math::distanceToLine(obstaclePos, from, p2);
    if (vec.dot(obstaclePos - from) >=0 && distance <= obstacle->getRadius() + radius) {
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
    const auto obstaclePos = EX::pos(*obstacle);
    /// ��������� ��������������� ������� �����������
    auto obstacleTangets = Math::tangetsForTwoCircle(from, radius, obstaclePos, obstacle->getRadius());
    assert(2 == obstacleTangets.size());
    setTangetsDirection(from, obstaclePos, obstacleTangets);

    for (const auto& tangent : obstacleTangets) {
      if (!isIntersectVectorWithGroup(from, tangent, obstaclePos, radius, group)) {
        /// ���������� �� ��������� ��������, ��� ������ ��� ������ ����������, �������� �� 0 �� 2
        double deviation = 1 - delta.dot(tangent);
        double length = abs((obstaclePos - from).dot(tangent));

        assert(resultIndex < 2); // ������ ������ ���������� 2
        result[resultIndex++] = {deviation, obstaclePos,  MAX(1, length), tangent};
        maxRadius = MAX(maxRadius, obstacle->getRadius());

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
