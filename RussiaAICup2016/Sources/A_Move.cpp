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


/// находит ближайшую группу, с которой пересекаеться вектор движения юнита.
const Obstacles* findNearestGroup(const Position& from, const double radius, const Position& to, const ObstaclesGroups& obstacles, double& minDistance) {
  const Obstacles* nearestGroup = nullptr;

  for (const auto& group : obstacles) {
    for (const auto& obstacle : group) {
      const auto obstaclePos = EX::pos(obstacle);
      const auto fullRadius = radius + obstacle.getRadius();

      /// проверяем перечение с препятствием, но увеличенного радиуса, так как пряпятствия стоят не притык друг к другу
      if (Math::distanceToLine(obstaclePos, to, from) < fullRadius) {
        /// пряпятствие за нашей спиной
        if ((to - from).dot(obstaclePos - from) < 0) {
          continue;
        }

        /// препятствие дальше точки окончания движения
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


/// устанавливает направление движения в сторону выхода если это не так (так как касательные могут иметь любой знак
void setTangetsDirection(const Position& from, const Position& obstaclePos, std::vector<Vector>& tangets) {
  assert(2 == tangets.size());
  const auto delta = (obstaclePos - from).normal();

  /// не ноль, чтобы навсякий случай не развернуть 90 градусов
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
    /// игнорируем самого себя
    if ((obstaclePos - checkPos).length2() < 0.1) {
      continue;
    }

    const double distance = Math::distanceToLine(obstaclePos, from, p2);
    if (vec.dot(obstaclePos - from) >=0 && distance < obstacle.getRadius() + radius) {
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
/// ищет края группы - точки в группе до которых вектор изменения от движения максимален. таких точки две - в разные стороны поворота
/// после чего для экономии времени, сразуже находит тот вариант который имеет минимальное отклонение, и выдает касательную нужной длины
/// проверяет также что юнит не находится в полном окружении - результат возращаеться как входящая переменная
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
    /// возращает нормализованные вектора касательных
    auto obstacleTangets = Math::tangetsForTwoCircle(from, radius, obstaclePos, obstacle.getRadius());
    assert(2 == obstacleTangets.size());
    setTangetsDirection(from, obstaclePos, obstacleTangets);

    for (const auto& tangent : obstacleTangets) {
      if (!isIntersectVectorWithGroup(from, tangent, obstaclePos, radius, group)) {
        /// отклонение от нормально движения, чем больше тем больше отклонение, интервал от 0 до 2
        double deviation = 1 - delta.dot(tangent);
        double length = abs((obstaclePos - from).dot(tangent));

        assert(resultIndex < 2); // всегда должно получаться 2
        result[resultIndex++] = {deviation, obstaclePos,  MAX(1, length), tangent};
        maxRadius = MAX(maxRadius, obstacle.getRadius());
      }
    }
  }

  if (0 == resultIndex) {
    /// нет возможности выйти из группы - окружен.
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
  const auto from = EX::pos(unit);

  Vector toIter = to;
  Vector tangent = to - from;
  bool found = false;

  ObstaclesGroups obstaclesGroups = obstacles;

  while (obstaclesGroups.size() > 0) {
    double minDistance = 4000;
    const Obstacles* nearestGroup = findNearestGroup(from, unit.getRadius(), toIter, obstaclesGroups, minDistance);
    if (nullptr == nearestGroup) { // препятствий на пути следования нет
      if (found) {
        tangent = tangent.normal() * minDistance;
      }
      break;
    }

    found = true;
    tangent = findGroupPartsAndReturnTangets(from, unit.getRadius(), to, *nearestGroup);

    /// находимся в окружении
    if (tangent.length() < 1.0e-9) {
      return Vector();
    }

    toIter = from + ((tangent.length() < minDistance) ? tangent : tangent.normal() * minDistance);

    // удаляем группу с которой уже пересеклись
    const size_t groupIndex = nearestGroup - &obstaclesGroups[0];
    obstaclesGroups.erase(obstaclesGroups.begin() + groupIndex);
  }

  return tangent;
}

Position simplePathToPosition(const Path& path) {
  assert(path.size() >= 2);

  if ((path[1] - path[0]).length() < 35/*TODO: magic constant*/ && path.size() >= 3) {
    return path[2];
  }

  return path[1];
}

Position adaptivePathToPosition(const Path& path, const model::Wizard& unit, const double radius) {
  assert(path.size() >= 1);

  const auto unitPos = Position(unit.getX(), unit.getY());

  const auto& lastPos = path[path.size() - 1];
  /// если последняя точка находиться в обзоре, то двигаемся к ней
  if ((unitPos - lastPos).length2() - 1/*а то иногда на границе может оказаться*/ <= radius*radius) {
    return lastPos;
  }

  for (size_t i = path.size()-1; i > 0; i--) { // первая точка включается из-за (i-1) в уравнении
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
  return pos - EX::pos(unit);
}

Vector Algorithm::move(const model::Wizard& unit, const Path& path) {
  const auto pos = simplePathToPosition(path);
  /// так как адаптивный путь строиться так чтобы как можно быстрее дойти на поворотах,
  /// не стоит применять его без учета препятствий, ибо они скорей всего будут, и мы в них уткнемся
  /// const auto pos = adaptivePathToPosition(path, unit);

  return pos - EX::pos(unit);
}

Vector Algorithm::move(const model::CircularUnit& unit, const Path& path, const ObstaclesGroups& obstacles) {
  const auto pos = simplePathToPosition(path);
  return move(unit, pos, obstacles);
}

Vector Algorithm::move(const model::Wizard& unit, const Path& path, const ObstaclesGroups& obstacles, double range) {
  const auto pos = adaptivePathToPosition(path, unit, (range < 0) ? unit.getVisionRange() : range);
  return move(unit, pos, obstacles);
}