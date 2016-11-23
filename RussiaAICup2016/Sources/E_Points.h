//
//File: E_Points.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//Renamed: 21/11/2016
//

#pragma once

#include "C_Singleton.h"
#include "C_Vector2D.h"
#include <vector>

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class Points: public Singleton<Points> {
  public:
    enum PointType {
      ACADEMY_BASE = 0,
      ACADEMY_BASE_TOP,
      ACADEMY_BASE_MIDDLE,
      ACADEMY_BASE_BOTTOM,

      RENEGADES_BASE,
      RENEGADES_BASE_TOP,
      RENEGADES_BASE_MIDDLE,
      RENEGADES_BASE_BOTTOM,

      ACADEMY_TOP_FIRST_TOWER,
      ACADEMY_TOP_SECOND_TOWER,
      RENEGADES_TOP_FIRST_TOWER,
      RENEGADES_TOP_SECOND_TOWER,

      ACADEMY_BOTTOM_FIRST_TOWER,
      ACADEMY_BOTTOM_SECOND_TOWER,
      RENEGADES_BOTTOM_FIRST_TOWER,
      RENEGADES_BOTTOM_SECOND_TOWER,


      ACADEMY_MIDDLE_FIRST_TOWER,
      ACADEMY_MIDDLE_SECOND_TOWER,
      RENEGADES_MIDDLE_FIRST_TOWER,
      RENEGADES_MIDDLE_SECOND_TOWER,

      BONUS_TOP,
      BONUS_BOTTOM,

      TOP_CENTER,
      BOTTOM_CENTER,
      MIDDLE_CENTER,

      /// additional

      CENTER_BONUS_TOP,
      CENTER_ACADEMY,
      CENTER_BONUS_BOTTOM,
      CENTER_RENEGADES,

      ACADEMY_TOP_CENTER,
      BONUS_TOP_CENTER,
      RENEGADES_TOP_CENTER,

      ACADEMY_BOTTOM_CENTER,
      BONUS_BOTTOM_CENTER,
      RENEGADES_BOTTOM_CENTER,

      _POINT_COUNT_
    };

  public:
    Points();

    static const Position& point(PointType type);

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
    void initPoints();

  private:
    std::vector<Position> points;
  };
}