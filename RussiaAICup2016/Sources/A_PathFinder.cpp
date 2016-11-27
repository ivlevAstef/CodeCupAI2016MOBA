#include "A_PathFinder.h"
#include "E_World.h"
#include "E_InfluenceMap.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "C_Logger.h"
#include <chrono>
#include <iostream>

using namespace AICup;
using namespace Algorithm;


Position PathConstants::toReal(Vector2D<int> point, double dx, double dy) {
  return Position((point.x + dx) * step, (point.y + dy) * step);
}
Vector2D<int> PathConstants::toInt(Position point) {
  return Vector2D<int>((int)floor(point.x / step), (int)floor(point.y / step));
}





Path::Path(Position from, Position to, const double radius):
  from(from), to(to), radius(radius) {
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
      int lastTSign = 0;
      /// �������� � ����� ������� ���� �����������
      for (size_t i = 1; i < count; i++) {
        const auto dir = path[i] - path[i - 1];
        const double t = (obstaclePos - path[i - 1]).dot(dir.normal());
        const int tSign = SIGN(t);
        if ((0 < t && t < dir.length()) || lastTSign == -tSign) {
          double alpha = (EX::pos(*obstacle) - path[i]).cross(dir);
          neightborsCount[alpha > 0 ? 0 : 1]++;
          lastTSign = 0;
        } else {
          lastTSign = tSign;
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
    calculateCost();
    /// ����������� ��� ���� �� ����� ��� �� ���������
    calculateWeight(fromByInt);

    /// ��� �������� �� ����� ������ ������ ������� ��������
  } else if (lastByInt != fromByInt) {
    LogAssert(0 == weights[lastByInt.x][lastByInt.y]);


    weights[fromByInt.x][fromByInt.y] = 0;
    fastCalculateWeight = fastCalculateWeight - 0.005f;
    weights[lastByInt.x][lastByInt.y] = fastCalculateWeight;
  }
}

void PathFinder::calculatePath(const Position& to, std::shared_ptr<Path>& path) const {
  auto newPath = std::make_shared<Path>(from, to, radius);
  path.swap(newPath);

  calculatePath(*path.get());
}

void PathFinder::calculateCost() {
  for (const auto& obstacle : obstacles) {
    float life = 1;
    if (EX::isTree(*obstacle)) {
      life = 2.5f * (float(obstacle->getLife()) / 12.0f); /// ������
    } else if (EX::isNeutral(*obstacle)) {
      life = 1000.0f; /// ������� ����� ������� ������������
    } else {
      life = 5000.0f; /// ������ ������ �� ��������
    }

    const auto oPos = EX::pos(*obstacle);
    const auto fullRadius = radius + obstacle->getRadius();

    Math::fillGrid(reinterpret_cast<float*>(costs), oPos.x, oPos.y, PathConstants::step, fullRadius, life);
  }


  const float* enemiesMap = InfluenceMap::instance().getEnemiesMap();
  for (size_t x = 0; x < PathConstants::memorySize; x++) {
    for (size_t y = 0; y < PathConstants::memorySize; y++) {
      const size_t mapX = size_t((x * double(PathConstants::step)) / double(InfluenceMapConstants::step));
      const size_t mapY = size_t((y * double(PathConstants::step)) / double(InfluenceMapConstants::step));
      const float value = enemiesMap[mapX * InfluenceMapConstants::memorySize + mapY];
      costs[x][y] += value;
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
  auto iFrom = PathConstants::toInt(path.from);
  auto iTo = PathConstants::toInt(path.to);
  /// �� � ������ ����� �� ������ �������� �� �������, �� ��������� ��������� ����� ������ ��� ���� ������ ���� ��������...
  iTo.x = MAX(1, MIN(iTo.x, PathConstants::memorySize - 2));
  iTo.y = MAX(1, MIN(iTo.y, PathConstants::memorySize - 2));
  iFrom.x = MAX(1, MIN(iFrom.x, PathConstants::memorySize - 2));
  iFrom.y = MAX(1, MIN(iFrom.y, PathConstants::memorySize - 2));

  path.count = 0;
  path.length = 0;

  if (weights[iTo.x][iTo.y] >= PathConstants::maxValue) {
    LogAssertMsg(false, "can't found path... really?");
    return;
  }


  Vector2D<int> iter = iTo;
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
  } while (iter != iFrom);
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
 /* if (Visualizator::ABS == visualizator.getStyle()) {
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