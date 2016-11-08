//
//File: E_Graph.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include "C_Singleton.h"
#include "C_Vector2D.h"
#include "model/LineType.h"
#include <vector>

namespace AICup
{
  class Graph: public Singleton<Graph> {
  private:
    typedef Position Point;

    struct Join {
      size_t p1Index;
      size_t p2Index;
      double weight;
    };

    typedef std::vector<size_t> JoinsForPoint;

    enum PointType {
      ACADEMY_BASE = 0,
      DEFIANT_BASE,
      ACADEMY_TOP_FIRST_TOWER,
      ACADEMY_TOP_SECOND_TOWER,
      DEFIANT_TOP_FIRST_TOWER,
      DEFIANT_TOP_SECOND_TOWER,

      ACADEMY_BOTTOM_FIRST_TOWER,
      ACADEMY_BOTTOM_SECOND_TOWER,
      DEFIANT_BOTTOM_FIRST_TOWER,
      DEFIANT_BOTTOM_SECOND_TOWER,


      ACADEMY_MIDDLE_FIRST_TOWER,
      ACADEMY_MIDDLE_SECOND_TOWER,
      DEFIANT_MIDDLE_FIRST_TOWER,
      DEFIANT_MIDDLE_SECOND_TOWER,

      BONUS_TOP,
      BONUS_BOTTOM,

      TOP_CENTER,
      BOTTOM_CENTER,
      MIDDLE_CENTER,

      _POINT_COUNT_
    };

  public:
    Graph();

    std::vector<Position> path(const Position& from, const Position& to);

    void update(); /// использует World

  private:
    void initDefaultGraph();
    void initDefaultPointMemory();
    void initDefaultJoinMemory();

    const size_t addPoint(const Position& pos);

    void updatePointsByMemory();

    std::vector<size_t> dijkstraPath(const size_t& fromPIndex, const size_t& toPIndex);

  private:
    std::vector<Join> joinMemory;
    std::vector<Point> pointMemory;

    std::vector<JoinsForPoint> joinsForPoints;
  };
}