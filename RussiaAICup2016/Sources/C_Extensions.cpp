//
//File: Extensions.cpp
//Description:
//Author: Ivlev Alexander. Stef
//Created: 10:16 28/12/2015
//Copyright (c) SIA 2015. All Right Reserved.
//


#include "C_Extensions.h"
#include <cmath>

using namespace AICup;


double Extensions::constPI() {
  static const double PI = std::atan(1) * 4;
  return PI;
}

double Extensions::distance(const Position& p, const Position& p1, const Position& p2) {
  const auto v = p2 - p1;
  const auto w = p - p1;

  const auto c1 = w.dot(v);
  if (c1 <= 0) {
    return (p - p1).length();
  }

  if (v.dot(v) <= c1) {
    return (p - p2).length();
  }

  return abs(v.normal().cross(w));
}

double Extensions::angleDiff(double angle1, double angle2) {
  double angle = angle1 - angle2;

  while (angle > AICUP_PI) {
    angle -= 2.0 * AICUP_PI;
  }

  while (angle < -AICUP_PI) {
    angle += 2.0 * AICUP_PI;
  }

  return angle;
}