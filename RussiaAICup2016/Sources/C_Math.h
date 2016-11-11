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

#define AICUP_PI Math::constPI()

    /// Дистация до отрезка! или перпендикуляр или начало/конец отрезка
    double distance(const Position& point, const Position& p1, const Position& p2);

    double angleDiff(double angle1, double angle2);


    Position perpendicularFromPointToLine(const Position& point, const Position& p1, const Position& p2);

    /// или точка куда падает перпендикуляр, или начало/конец отрезка
    Position intersectLineFromPoint(const Position& point, const Position& p1, const Position& p2);


    std::vector<Position> intersectLineWithCircle(const Position& point, double radius, const Position& p1, const Position& p2);
  }
}