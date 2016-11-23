#include "C_Math.h"
#include <cmath>

using namespace AICup;

double Math::constPI() {
  static const double PI = std::atan(1) * 4;
  return PI;
}

double Math::angleDiff(double angle1, double angle2) {
  double angle = angle1 - angle2;

  while (angle > AICUP_PI) {
    angle -= 2.0 * AICUP_PI;
  }

  while (angle < -AICUP_PI) {
    angle += 2.0 * AICUP_PI;
  }

  return angle;
}

double Math::distanceToLine(const Position& point, const Position& p1, const Position& p2) {
  Vector delta = p2 - p1;
  Vector distanceToP1 = point - p1;
  return abs(delta.normal().cross(distanceToP1));
}

double Math::distanceToSegment(const Position& point, const Position& p1, const Position& p2) {
  Vector delta = p2 - p1;
  Vector distanceToP1 = point - p1;
  double t = delta.normal().dot(distanceToP1);
  t = MAX(0, MIN(t, delta.length()));

  return (point - (p1 + delta.normal() * t)).length();
}

Position Math::point_distanceToLine(const Position& point, const Position& p1, const Position& p2) {
  Vector delta = p2 - p1;
  Vector distanceToP1 = point - p1;
  double t = delta.normal().dot(distanceToP1);

  return p1 + delta.normal() * t;
}

Position Math::point_distanceToSegment(const Position& point, const Position& p1, const Position& p2) {
  Vector delta = p2 - p1;
  Vector distanceToP1 = point - p1;
  double t = delta.normal().dot(distanceToP1);
  t = MAX(0, MIN(t, delta.length()));

  return p1 + delta.normal() * t;
}



std::vector<Position> Math::intersectSegmentWithCircle(const Position& point, const double radius, const Position& p1, const Position& p2) {
  const auto perpedicularPos = point_distanceToLine(point, p1, p2);
  const auto perpendicularV = point - perpedicularPos;

  const auto radius2 = radius*radius;
  const auto distance2 = perpendicularV.length2();
  const auto len2 = radius2 - distance2;

  /// линия очень далеко
  if (len2 < 0) {
    return std::vector<Position>();
  } if (len2 < 1.0e-3) { // касается
    return {perpedicularPos};
  }


  Vector translateVector;
  if (distance2 < 1.0e-3) { // линия проходит через центр
    translateVector = (p2 - p1).normal() * radius;
  } else {
    const auto len = sqrt(len2);
    translateVector = perpendicularV.perpendicular().normalize() * len;
  }

  const auto rP1 = perpedicularPos + translateVector;
  const auto rP2 = perpedicularPos - translateVector;

  bool rP1IsLine = (p1 - rP1).dot(p2 - rP1) <= 0;
  bool rP2IsLine = (p1 - rP2).dot(p2 - rP2) <= 0;

  std::vector<Position> result;

  if (rP1IsLine) {
    result.push_back(rP1);
  }

  if (rP2IsLine) {
    result.push_back(rP2);
  }

  return result;
}

Position Math::center(const Position& p1, const Position& p2) {
  return p1 + (p2 - p1) * 0.5;
}

Position Math::center(const double x1, const double y1, const double x2, const double y2) {
  return Position(x1 + (x2 - x1) * 0.5, y1 + (y2 - y1) * 0.5);
}

std::vector<Vector> Math::tangetsForTwoCircle(const Position& p1, const double radius1, const Position& p2, const double radius2) {
  /// всегда считаем что первый радиус больше второго
  if (radius1 < radius2) {
    return tangetsForTwoCircle(p2, radius2, p1, radius1);
  }

  const double radius = radius1 + radius2;
  const double radiusX2 = radius * radius;
  const auto delta = p2 - p1;
  const auto deltaLength = delta.length();

  /// два круга находяться очень близко -> касательная = перпендикуляру к вектору
  if (deltaLength < radius + 1.0e-3) {
    const auto n = delta.perpendicular().normal();
    return {n, -n};
  }


  const auto vLength = radiusX2 / deltaLength;
  const auto v = delta.normal() * vLength;

  const auto nLength = sqrt(radiusX2 - vLength * vLength);
  const auto n = delta.perpendicular().normal() * nLength;

  const auto tangent1 = (p1 - p2) + v + n;
  const auto tangent2 = (p1 - p2) + v - n;

  return{tangent1.normal(), tangent2.normal()};
}

void Math::fillGrid(float* grid, const double x, const double y, const double step, const double radius, const float value) {
  const size_t size = 4000 / step;

  const double minRealX = MAX(0, x - radius) / step;
  const double maxRealX = MIN(4000, x + radius) / step;
  const double minRealY = MAX(0, y - radius) / step;
  const double maxRealY = MIN(4000, y + radius) / step;

  const size_t minX = floor(minRealX);
  const size_t maxX = ceil(maxRealX);
  const size_t minY = floor(minRealY);
  const size_t maxY = ceil(maxRealY);

  for (size_t x = minX; x < maxX; x++) {
    const float sxMin = 1 - MAX(0, minRealX - double(x));
    const float sxMax = 1 - MAX(0, double(x + 1) - maxRealX);
    for (size_t y = minY; y < maxY; y++) {
      const float syMin = 1 - MAX(0, minRealY - double(y));
      const float syMax = 1 - MAX(0, double(y + 1) - maxRealY);

      const float dx = sxMin + sxMax - 1;
      const float dy = syMin + syMax - 1;

      grid[x * size + y] += dx*dx*dy*dy * value;
    }
  }
}