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