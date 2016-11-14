//
//File: CM_Move.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "CM_Move.h"
#include "E_Graph.h"
#include "C_Math.h"
#include "E_Game.h"
#include <tuple>
#include <algorithm>

using namespace AICup;

std::vector<Position> Move::path(const Position from, const Position to, double& length) {
  return Graph::instance().path(from, to, length);
}

MoveAction Move::move(const model::CircularUnit& unit, const Position& to, const double speedLimit, MoveStyle style) {
  const auto dx = to.x - unit.getX();
  const auto dy = to.y - unit.getY();
  auto speedVec = Vector(dx, -dy).normalize().rotated(unit.getAngle());
  speedVec.y *= -1;
  if (-1.0e-3 + SPEED_LIMIT_NOT_SET < speedLimit && speedLimit < SPEED_LIMIT_NOT_SET + 1.0e-3) {
    speedVec *= Game::instance().model().getWizardForwardSpeed();
  } else {
    speedVec *= speedLimit;
  }


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

/// находит ближайшую группу, с которой пересекаеться вектор движения юнита.
const Obstacles* findNearestGroup(const Position& from, const double radius, const Position& to, const ObstaclesGroups& obstacles) {
  double minDistance = 100000;
  const Obstacles* nearestGroup = nullptr;

  for (const auto& group : obstacles) {
    for (const auto& obstacle : group) {
      const auto obstaclePos = Position(obstacle.getX(), obstacle.getY());

      /// проверяем перечение с препятствием, но увеличенного радиуса, так как пряпятствия стоят не притык друг к другу
      if (Math::distanceToLine(obstaclePos, to, from) < obstacle.getRadius() + radius) {
        /// пряпятствие за нашей спиной
        if ((to - from).dot(obstaclePos - from) < 0) {
          continue;
        }

        /// препятствие дальше точки окончания движения
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
  const Position p2 = from + vec * 10000;
  for (const auto& obstacle : group) {
    const auto obstaclePos = Position(obstacle.getX(), obstacle.getY());
    /// игнорируем самого себя
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
  Position obstaclePos;
  Vector tangent;
};
/// ищет края группы - точки в группе до которых вектор изменения от движения максимален. таких точки две - в разные стороны поворота
/// после чего для экономии времени, сразуже находит тот вариант который имеет минимальное отклонение, и выдает касательную нужной длины
/// проверяет также что юнит не находится в полном окружении - результат возращаеться как входящая переменная
Vector findGroupPartsAndReturnTangets(const Position& from, const double radius, const Position& to, const Obstacles& group) {
  assert(!group.empty());

  const auto delta = (to - from).normal();

  /// deviations, obstacle, tangent
  std::vector<ObstacleData> dataArr;
  dataArr.reserve(group.size() * 2);

  for (const auto& obstacle : group) {
    const auto obstaclePos = Position(obstacle.getX(), obstacle.getY());
    auto obstacleTangets = Math::tangetsForTwoCircle(from, radius, obstaclePos, obstacle.getRadius());
    assert(2 == obstacleTangets.size());
    setTangetsDirection(from, obstaclePos, obstacleTangets);

    for (const auto& tanget : obstacleTangets) {
      /// отклонение от нормально движения, чем больше тем больше отклонение, интервал от 0 до 2
      double deviation = 1 - delta.dot(tanget);
      dataArr.push_back(ObstacleData{deviation, obstaclePos, tanget});
    }
  }

  std::sort(dataArr.begin(), dataArr.end(), [] (ObstacleData a, ObstacleData b) {
    return a.deviation < b.deviation;
  });


  /// идем от меньшего отклонения к большему, и проверяем что нет пересечений.
  for (const ObstacleData& data : dataArr) {
    const Position& obstaclePos = data.obstaclePos;
    const Vector& tangent = data.tangent;

    if (!isIntersectVectorWithGroup(from, tangent, obstaclePos, radius, group)) {
      return tangent.normal() * (obstaclePos - from).length();
    }

  }

  /// нет возможности выйти из группы - окружен.
  return Vector();
}

MoveAction Move::move(const model::CircularUnit& unit, const Position& to, const ObstaclesGroups& obstacles, const double speedLimit, MoveStyle style) {
  const auto from = Position(unit.getX(), unit.getY());

  Position iterTo = to;

  ObstaclesGroups obstaclesGroups = obstacles;

  while (obstaclesGroups.size() > 0) {
    const Obstacles* nearestGroup = findNearestGroup(from, unit.getRadius(), iterTo, obstaclesGroups);
    if (nullptr == nearestGroup) { // препятствий на пути следования нет
      break;
    }

    const auto tanget = findGroupPartsAndReturnTangets(from, unit.getRadius(), to, *nearestGroup);
    /// находимся в окружении
    if (tanget.length2() < 1.0e-9) {
      return MoveAction{0,0,0, true};
    }


    iterTo = from + tanget;

    const size_t groupIndex = (nearestGroup - &obstaclesGroups[0]) / sizeof(Obstacles);
    obstaclesGroups.erase(obstaclesGroups.begin() + groupIndex);
  };


  return move(unit, iterTo, speedLimit, style);
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


MoveAction Move::move(const model::CircularUnit& unit, const Path& path, const double speedLimit, MoveStyle style) {
  const auto pos = simplePathToPosition(path);
  return move(unit, pos, speedLimit, style);
}

MoveAction Move::move(const model::Wizard& unit, const Path& path, const double speedLimit, MoveStyle style) {
  const auto pos = simplePathToPosition(path);
  /// так как адаптивный путь строиться так чтобы как можно быстрее дойти на поворотах,
  /// не стоит применять его без учета препятствий, ибо они скорей всего будут, и мы в них уткнемся
  /// const auto pos = adaptivePathToPosition(path, unit);

  return move(unit, pos, speedLimit, style);
}

MoveAction Move::move(const model::CircularUnit& unit, const Path& path, const ObstaclesGroups& obstacles, const double speedLimit, MoveStyle style) {
  const auto pos = simplePathToPosition(path);
  return move(unit, pos, obstacles, speedLimit, style);
}

MoveAction Move::move(const model::Wizard& unit, const Path& path, const ObstaclesGroups& obstacles, const double speedLimit, MoveStyle style) {
  const auto pos = adaptivePathToPosition(path, unit);
  return move(unit, pos, obstacles, speedLimit, style);
}