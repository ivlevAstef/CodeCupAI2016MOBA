//
//File: Math.h
//Description:
//Author: Ivlev Alexander. Stef
//Created: 21:21 12/11/2016
//Copyright (c) SIA 2016. All Right Reserved.
//
#pragma once

#include "C_Vector2D.h"
#include <vector>

namespace AICup
{
  namespace Math
  {
    double constPI();

    template <typename T> int sign(T val) {
      return (T(0) < val) - (val < T(0));
    }

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define ABS(a) (((a) > 0) ? (a) : -(a))
#define SIGN(a) (((a) < 0) ? -1 : 1)

#define AICUP_PI Math::constPI()

    double angleDiff(double angle1, double angle2);

    // line - infinite
    // segment - from p1 to p2

    double distanceToLine(const Position& point, const Position& p1, const Position& p2);
    double distanceToSegment(const Position& point, const Position& p1, const Position& p2);

    Position point_distanceToLine(const Position& point, const Position& p1, const Position& p2);
    Position point_distanceToSegment(const Position& point, const Position& p1, const Position& p2);

    std::vector<Position> intersectSegmentWithCircle(const Position& point, const double radius, const Position& p1, const Position& p2);

    Position center(const Position& p1, const Position& p2);
    Position center(const double x1, const double y1, const double x2, const double y2);

    /// касательная между двумя сторонами круга с разных сторон!!!
    std::vector<Vector> tangetsForTwoCircle(const Position& p1, const double radius1, const Position& p2, const double radius2);

    void fillGrid(float* grid, const double x, const double y, const double step, const double radius, const float value);
  }
}