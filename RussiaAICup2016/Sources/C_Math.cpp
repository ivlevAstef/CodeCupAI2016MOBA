#include "C_Math.h"
#include <cmath>

using namespace AICup;

double Math::constPI() {
  static const double PI = std::atan(1) * 4;
  return PI;
}

double Math::distance(const Position& point, const Position& p1, const Position& p2) {
  const auto pPoint = intersectLineFromPoint(point, p1, p2);
  return (point - pPoint).length();
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

Position Math::perpendicularFromPointToLine(const Position& point, const Position& p1, const Position& p2) {
  Vector delta = p2 - p1;
  Vector distanceToP1 = point - p1;
  double t = delta.normal().dot(distanceToP1);

  return p1 + delta.normal() * t;
}

Position Math::intersectLineFromPoint(const Position& point, const Position& p1, const Position& p2) {
  Vector delta = p2 - p1;
  Vector distanceToP1 = point - p1;
  double t = delta.normal().dot(distanceToP1);
  t = MAX(0, MIN(t, delta.length()));

  return p1 + delta.normal() * t;
}

std::vector<Position> Math::intersectLineWithCircle(const Position& point, double radius, const Position& p1, const Position& p2) {
  const auto perpedicularPos = perpendicularFromPointToLine(point, p1, p2);
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

  bool rP1IsLine = (p1 - rP1).dot(p2 - rP1) < 0;
  bool rP2IsLine = (p1 - rP2).dot(p2 - rP2) < 0;

  std::vector<Position> result;

  if (rP1IsLine) {
    result.push_back(rP1);
  }

  if (rP2IsLine) {
    result.push_back(rP2);
  }

  return result;
}