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
    struct Point {
      double x;
      double y;
    };

    struct Join {
      const Point* p1;
      const Point* p2;
      const double weight;

      Join(const Point* p1, const Point* p2);
    };

    struct PointWithJoins {
      const Point* point;
      std::vector<const Join*> joins;
    };

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

    const PointWithJoins* addPoint(const Position& pos);

    void updatePointsByMemory();

    std::vector<const PointWithJoins*> path(const PointWithJoins* from, const PointWithJoins* to);

  private:
    std::vector<Join> joinMemory;
    std::vector<Point> pointMemory;

    std::vector<PointWithJoins> points;
  };
}