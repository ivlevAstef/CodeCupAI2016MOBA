#include "A_PathFinder.h"
#include "E_World.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include <chrono>
#include <iostream>

using namespace AICup;
using namespace Algorithm;

void PathFinder::setTo(const Position& to) {
  this->to = to;
}


Position PathFinder::calculate(const model::CircularUnit& unit, const double visionRange) {
  const auto t1 = std::chrono::high_resolution_clock::now();

  const auto from = EX::pos(unit);
  const double radius = unit.getRadius();

  for (size_t x = 0; x < memorySize; x++) {
    for (size_t y = 0; y < memorySize; y++) {
      weights[x][y] = maxValue;
      costs[x][y] = 1;
    }
  }

  /// по краям переходы стоят очень дорого, чтобы нельзя было за них зайти
  for (size_t i = 0; i < memorySize; i++) {
    costs[i][0] = maxValue + 1;
    costs[0][i] = maxValue + 1;
    costs[i][memorySize - 1] = maxValue + 1;
    costs[memorySize - 1][i] = maxValue + 1;
  }

  const auto toByInt = toInt(to);
  const auto fromByInt = toInt(from);

  calculateCost(World::instance().obstacles(unit, (to - from).length(), true), radius);
  calculateWeight(toByInt);
  findPath(fromByInt, toByInt);

  const auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> fs = t2 - t1;
  std::cout << fs.count() << std::endl;
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(fs).count() << std::endl;
  return calculateNearestCurvaturePoint(from, to, visionRange);
}

void PathFinder::calculateCost(const Obstacles& obstacles, const double radius) {
  for (const auto& obstacle : obstacles) {
    const auto obstaclePos = EX::pos(obstacle);
    const auto fullRadius = radius + obstacle.getRadius();

    const double minRealX = (obstaclePos.x - fullRadius) / step;
    const double maxRealX = (obstaclePos.x + fullRadius) / step;
    const double minRealY = (obstaclePos.y - fullRadius) / step;
    const double maxRealY = (obstaclePos.y + fullRadius) / step;

    const size_t minX = floor(minRealX);
    const size_t maxX = ceil(maxRealX);
    const size_t minY = floor(minRealY);
    const size_t maxY = ceil(maxRealY);

    for (size_t x = minX; x < maxX; x++) {
      double sXMin = 1 - MAX(0, minRealX - double(x));
      double sXMax = 1 - MAX(0, double(x + 1) - maxRealX);
      for (size_t y = minY; y < maxY; y++) {
        double syMin = 1 - MAX(0, minRealY - double(y));
        double syMax = 1 - MAX(0, double(y + 1) - maxRealY);

        costs[x][y] += (sXMin + sXMax)*(syMin + syMax) * (obstacle.getLife() / 12);
      }
    }
  }
}

void PathFinder::calculateWeight(Vector2D<int> to) {
  struct Neighbors {
    Vector2D<int> vec;
    float mult;
  };
  static const Neighbors neighbors[8] = {
    Neighbors{{-1,  0}, 1},
    Neighbors{{ 0, -1}, 1},
    Neighbors{{ 1,  0}, 1},
    Neighbors{{ 0,  1}, 1},
    Neighbors{{ 1,  1}, sqrtf(2)},
    Neighbors{{ 1, -1}, sqrtf(2)},
    Neighbors{{-1, -1}, sqrtf(2)},
    Neighbors{{-1,  1}, sqrtf(2)}
  };

  int areaBegin = 0;
  int areaEnd = 1;

  area[areaBegin].set(to.x, to.y);
  weights[area[areaBegin].x][area[areaBegin].y] = 0;

  do {
    int newAreaEnd = areaEnd;
    int areaIndex = areaBegin;
    while (areaIndex != areaEnd) {
      const int x = area[areaIndex].x;
      const int y = area[areaIndex].y;
      const int weight = weights[x][y];

      for (const auto& neighbor : neighbors) {
        const int nX = x + neighbor.vec.x;
        const int nY = y + neighbor.vec.y;
        const float w = weight + costs[nX][nY] * neighbor.mult;
        if (weights[nX][nY] > w) {
          weights[nX][nY] = w;

          area[newAreaEnd++] = {nX, nY};
        }
      }

      areaIndex = (areaIndex + 1) % areaSize;
    }

    areaBegin = areaEnd;
    areaEnd = newAreaEnd % areaSize;
  } while (areaBegin != areaEnd); // пока не посчитаем всю карту
}

void PathFinder::findPath(Vector2D<int> from, Vector2D<int> to) {
  static const Vector2D<int> neighbors[8] = {
    Vector2D<int>(-1,  0),
    Vector2D<int>(0, -1),
    Vector2D<int>(1,  0),
    Vector2D<int>(0,  1),
    Vector2D<int>(1,  1),
    Vector2D<int>(1, -1),
    Vector2D<int>(-1, -1),
    Vector2D<int>(-1,  1)
  };

  pathSize = 0;

  if (weights[from.x][from.y] >= maxValue) {
    assert(false && "can't found path... really?");
    return;
  }

  Vector2D<int> iter = from;
  do {
    const Vector2D<int> pos = iter;
    int minWeight = maxValue;

    for (const auto& neighbor : neighbors) {
      const int nX = pos.x + neighbor.x;
      const int nY = pos.y + neighbor.y;
      if (weights[nX][nY] < minWeight) {
        minWeight = weights[nX][nY];
        iter.set(nX, nY);
      }
    }

    path[pathSize++] = iter;
  } while (iter != to);
}

Position PathFinder::calculateNearestCurvaturePoint(const Position& from, Position to, const double radius) {
  /// первую точку проверяем с вещественной конечной точкой, а потом уже их смещаем
  for (size_t i = 1; i < pathSize - 1; i++) {
    const auto p1 = toReal(path[pathSize-i], 0.5, 0.5);
    const auto p2 = toReal(path[pathSize-i-1], 0.5, 0.5);

    const auto intersections = Math::intersectSegmentWithCircle(from, radius, p1, p2);
    if (1 == intersections.size()) {
      return intersections[0];
    } else if (2 == intersections.size()) {
      return (intersections[0] - from).length2() > (intersections[1] - from).length2() ? intersections[0]: intersections[1];
    }
  }

  return to;
}

Position PathFinder::toReal(Vector2D<int> point, double dx, double dy) const {
  return Position((point.x + dx) * step, (point.y + dy) * step);
}
Vector2D<int> PathFinder::toInt(Position point) const {
  return Vector2D<int>(floor(point.x / step), floor(point.y / step));
}

#ifdef ENABLE_VISUALIZATOR
void PathFinder::visualization(const Visualizator& visualizator) const {
  for (size_t i = 1; i < pathSize; i++) {
    const auto p1 = toReal(path[i-1], 0.5, 0.5);
    const auto p2 = toReal(path[i], 0.5, 0.5);
    visualizator.line(p1.x, p1.y, p2.x, p2.y, 0x00ff00);
  }
}
#endif // ENABLE_VISUALIZATOR