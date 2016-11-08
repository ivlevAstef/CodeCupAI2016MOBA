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

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

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
      ACADEMY_BASE_TOP,
      ACADEMY_BASE_MIDDLE,
      ACADEMY_BASE_BOTTOM,

      DEFIANT_BASE,
      DEFIANT_BASE_TOP,
      DEFIANT_BASE_MIDDLE,
      DEFIANT_BASE_BOTTOM,

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

      /// additional


      CENTER_BONUS_TOP,
      CENTER_ACADEMY,
      CENTER_BONUS_BOTTOM,
      CENTER_DEFIANT,

      ACADEMY_TOP_CENTER,
      BONUS_TOP_CENTER,
      DEFIANT_TOP_CENTER,

      ACADEMY_BOTTOM_CENTER,
      BONUS_BOTTOM_CENTER,
      DEFIANT_BOTTOM_CENTER,

      _POINT_COUNT_
    };

  public:
    Graph();

    std::vector<Position> path(const Position& from, const Position& to, double& length);

    void update(); /// использует World

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
    void initDefaultGraph();
    void initDefaultPointMemory();
    void initDefaultJoinMemory();

    const size_t addPoint(const Position& pos);
    void adaptPoints(const size_t p1Index, const size_t p2Index);

    void updatePointsByMemory();

    std::vector<size_t> dijkstraPath(const size_t& fromPIndex, const size_t& toPIndex, double& length);

  private:
    std::vector<Join> joinMemory;
    std::vector<Point> pointMemory;

    std::vector<JoinsForPoint> joinsForPoints;
  };
}