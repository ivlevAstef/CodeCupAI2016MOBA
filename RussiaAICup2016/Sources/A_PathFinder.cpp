#include "A_PathFinder.h"
#include "E_World.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include <chrono>
#include <iostream>

using namespace AICup;
using namespace Algorithm;


Position PathConstants::toReal(Vector2D<int> point, double dx, double dy) {
  return Position((point.x + dx) * step, (point.y + dy) * step);
}
Vector2D<int> PathConstants::toInt(Position point) {
  return Vector2D<int>(floor(point.x / step), floor(point.y / step));
}





Path::Path(Position from, Position to, const double radius, const Obstacles& obstacles):
  from(from), to(to), radius(radius), obstacles(obstacles) {
  length = 0;
  count = 0;
}

/// ��������� ��� ������ ���������� ������.
/// ��� ����� ���� ��������� ��� ���� ������� ����� � ��� �� ������ ������� �� ����
bool Path::checkRemoved(const Obstacles& obstacles, const model::LivingUnit* obstacleForRemove) const {
  size_t neightborsCount[2] = {0, 0};

  for (const auto& obstacle : obstacles) {
    const auto fullRadius = obstacleForRemove->getRadius() + 2 * radius + obstacle->getRadius();

    /// ������� ������� �������
    if ( obstacle->getId() != obstacleForRemove->getId()
      && obstacle->getDistanceTo(*obstacleForRemove) < fullRadius) {
      const auto obstaclePos = EX::pos(*obstacle);
      /// �������� � ����� ������� ���� �����������
      for (size_t i = 1; i < count; i++) {
        const auto dir = path[i] - path[i - 1];
        double t = (obstaclePos - path[i-1]).dot(dir.normal());
        if (0 < t && t < dir.length()) {
          double alpha = (EX::pos(*obstacle) - path[i]).cross(dir);
          neightborsCount[alpha > 0 ? 0 : 1]++;
        }
      }
    }
  }

  return neightborsCount[0] >= 1 && neightborsCount[1] >= 1;
}

Obstacles Path::removeObstacles(Obstacles& obstacles) const {
  Obstacles result;

  for (size_t i = 1; i < count; i++) {

    /// ������� ��������� ������ � ��������.
    int obstacleRemoveIndex = -1;
    double minDistance = 88888;
    for (size_t obstacleIndex = 0; obstacleIndex < obstacles.size(); obstacleIndex++) {
      const auto& obstacle = obstacles[obstacleIndex];
      if (!(EX::isTree(*obstacle) || EX::isNeutral(*obstacle))) {
        continue;
      }

      const auto obstaclePos = EX::pos(*obstacle);
      const auto fullRadius = radius + obstacle->getRadius();

      const double distance = Math::distanceToSegment(obstaclePos, path[i - 1], path[i]);
      if (distance < minDistance && distance < fullRadius) {
        obstacleRemoveIndex = obstacleIndex;
        minDistance = distance;
      }
    }

    /// ���� ���� ������ ��� ��������, � ��� ����� �������
    if (obstacleRemoveIndex > 0 && checkRemoved(obstacles, obstacles[obstacleRemoveIndex])) {
      result.push_back(obstacles[obstacleRemoveIndex]);
      obstacles.erase(obstacles.begin() + obstacleRemoveIndex);
    }
  }

  return result;
}


Position Path::calculateNearestCurvaturePoint(const double visionRange) const {
  /// ������� ����� ������� ����������� � �����������
  for (size_t i = 1; i < count; i++) {
    const auto intersections = Math::intersectSegmentWithCircle(from, visionRange, path[i-1], path[i]);
    if (1 == intersections.size()) {
      return intersections[0];
    } else if (2 == intersections.size()) {
      return (intersections[0] - from).length2() >(intersections[1] - from).length2() ? intersections[0] : intersections[1];
    }
  }

  return to;
}




PathFinder::PathFinder() {
  lastCalculateTick = -10000;
}

void PathFinder::calculate(const model::CircularUnit& unit) {
  const auto lastByInt = PathConstants::toInt(from);

  from = EX::pos(unit);
  radius = unit.getRadius();
  const auto fromByInt = PathConstants::toInt(from);

  /// ��� � ��������� ����� ��������� ����� �������
  if (lastCalculateTick + 30 < World::model().getTickIndex()) {
    lastCalculateTick = World::model().getTickIndex();
    fastCalculateWeight = 1;

    clean();

    obstacles = World::instance().allObstacles(unit, true);
    calculateCost(obstacles, unit.getRadius());
    /// ����������� ��� ���� �� ����� ��� �� ���������
    calculateWeight(fromByInt);

    /// ��� �������� �� ����� ������ ������ ������� ��������
  } else if (lastByInt != fromByInt) {
    assert(0 == weights[lastByInt.x][lastByInt.y]);

    weights[fromByInt.x][fromByInt.y] = 0;
    fastCalculateWeight = fastCalculateWeight - 0.005;
    weights[lastByInt.x][lastByInt.y] = fastCalculateWeight;
  }
}

void PathFinder::calculatePath(const Position& to, Path** path) const {
  (*path) = new Path(from, to, radius, obstacles);

  calculatePath(**path);
}

void PathFinder::calculateCost(const Obstacles& obstacles, const double radius) {
  for (const auto& obstacle : obstacles) {
    double life = 1;
    if (EX::isTree(*obstacle)) {
      life = 2.0 * (obstacle->getLife() / 12); /// ������
    } else if (EX::isNeutral(*obstacle)) {
      life = PathConstants::maxValue; /// ������� ����� ������� ������������
    } else {
      life = PathConstants::maxValue; /// ������
    }

    const auto obstaclePos = EX::pos(*obstacle);
    const auto fullRadius = radius + obstacle->getRadius();

    const double minRealX = (obstaclePos.x - fullRadius) / PathConstants::step;
    const double maxRealX = (obstaclePos.x + fullRadius) / PathConstants::step;
    const double minRealY = (obstaclePos.y - fullRadius) / PathConstants::step;
    const double maxRealY = (obstaclePos.y + fullRadius) / PathConstants::step;

    const size_t minX = floor(minRealX);
    const size_t maxX = ceil(maxRealX);
    const size_t minY = floor(minRealY);
    const size_t maxY = ceil(maxRealY);

    for (size_t x = minX; x < maxX; x++) {
      double sxMin = 1 - MAX(0, minRealX - double(x));
      double sxMax = 1 - MAX(0, double(x + 1) - maxRealX);
      for (size_t y = minY; y < maxY; y++) {
        double syMin = 1 - MAX(0, minRealY - double(y));
        double syMax = 1 - MAX(0, double(y + 1) - maxRealY);

        costs[x][y] += sqrt(sxMin + sxMax - 1)*sqrt(syMin + syMax - 1) * life;

        assert(sxMin >= 0 && sxMax >= 0 && syMin >= 0 && syMax >= 0);
      }
    }
  }
}

void PathFinder::calculateWeight(Vector2D<int> to) {
  static const size_t areaSize = PathConstants::maxPath;
  static Vector2D<int> area[areaSize];
  static const Vector2D<int> neighbors[4] = {
    {-1,  0},
    { 0, -1},
    { 1,  0},
    { 0,  1},
  };

  int areaBegin = 0;
  int areaEnd = 1;

  area[areaBegin].set(to.x, to.y);
  weights[to.x][to.y] = 0;

  double costMult = 1; //����������� ���, ����� ��������� �������� ������ ���� �� ������
  do {
    int newAreaEnd = areaEnd;
    int areaIndex = areaBegin;

    while (areaIndex != areaEnd) {
      const int& x = area[areaIndex].x;
      const int& y = area[areaIndex].y;
      const float& weight = weights[x][y];

      for (const auto& neighbor: neighbors) {
        const int nX = x + neighbor.x;
        const int nY = y + neighbor.y;
        const float& cost = costs[nX][nY];
        if (weights[nX][nY] > weight + cost) {
          weights[nX][nY] = weight + cost;

          area[newAreaEnd].set(nX, nY);
          newAreaEnd = (newAreaEnd + 1) % areaSize;
        }
      }

      areaIndex = (areaIndex + 1) % areaSize;
    }

    areaBegin = areaEnd;
    areaEnd = newAreaEnd;
  } while (areaBegin != areaEnd); // ���� �� ��������� ��� �����
}

void PathFinder::calculatePath(Path& path) const {
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

  // ����������� �����
  const auto from = PathConstants::toInt(path.from);
  const auto to = PathConstants::toInt(path.to);

  path.count = 0;
  path.length = 0;

  if (weights[to.x][to.y] >= PathConstants::maxValue) {
    assert(false && "can't found path... really?");
    return;
  }


  Vector2D<int> iter = to;
  Position lastPos = path.to;
  do {
    const Vector2D<int> pos = iter;
    float minWeight = weights[pos.x][pos.y];

    for (const auto& neighbor : neighbors) {
      const int nX = pos.x + neighbor.x;
      const int nY = pos.y + neighbor.y;
      if (weights[nX][nY] < minWeight) {
        minWeight = weights[nX][nY];
        iter.set(nX, nY);
      }
    }

    const auto realPos = PathConstants::toReal(iter, 0.5, 0.5);
    path.path[path.count++] = realPos;
    path.length += (realPos - lastPos).length();

    lastPos = realPos;
  } while (iter != from);
}


void PathFinder::clean() {
  for (size_t x = 0; x < PathConstants::memorySize; x++) {
    for (size_t y = 0; y < PathConstants::memorySize; y++) {
      weights[x][y] = PathConstants::maxValue;
      costs[x][y] = 1;
    }
  }

  /// �� ����� �������� ����� ����� ������, ����� ������ ���� �� ��� �����
  for (size_t i = 0; i < PathConstants::memorySize; i++) {
    costs[i][0] = PathConstants::maxValue + 1;
    costs[0][i] = PathConstants::maxValue + 1;
    costs[i][PathConstants::memorySize - 1] = PathConstants::maxValue + 1;
    costs[PathConstants::memorySize - 1][i] = PathConstants::maxValue + 1;
  }
}

#ifdef ENABLE_VISUALIZATOR
void Path::visualization(const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    for (size_t i = 1; i < count; i++) {
      const auto& p1 = path[i-1];
      const auto& p2 = path[i];
      visualizator.line(p1.x, p1.y, p2.x, p2.y, 0x00ff00);
    }
  }
}


void PathFinder::visualization(const Visualizator& visualizator) const {
  /*if (Visualizator::ABS == visualizator.getStyle()) {
    for (int x = 0; x < PathConstants::memorySize; x++) {
      for (int y = 0; y < PathConstants::memorySize; y++) {

        const double weight = weights[x][y] / 200;
        int color = 255 - MIN(255, (int)(weight * 255));
        visualizator.line(x, y, x, y, color << 8);
      }
    }
  }*/


  /*if (Visualizator::PRE == visualizator.getStyle()) {
    for (int x = 0; x < PathConstants::memorySize - 1; x++) {
      for (int y = 0; y < PathConstants::memorySize - 1; y++) {
        const auto p1 = PathConstants::toReal({x,y}, 0, 0);
        const auto p2 = PathConstants::toReal({x,y}, 1, 1);

        const double weight = weights[x][y];
        int color = 255 - MIN(255, (int)(weight));
        visualizator.fillRect(p1.x, p1.y, p2.x, p2.y, color);
      }
    }
  }*/
}
#endif // ENABLE_VISUALIZATOR