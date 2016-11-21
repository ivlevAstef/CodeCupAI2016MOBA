//
//File: A_PathFinder.h
//Author: Ivlev Alexander. Stef
//Created: 21/11/2016
//


#pragma once

#include "E_Types.h"
#include "C_Vector2D.h"
#include "model\CircularUnit.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif


namespace AICup
{
  namespace Algorithm
  {
    class PathFinder {
    public:
      void setTo(const Position& to);

      ///Возращает точку куда надо идти
      Position calculate(const model::CircularUnit& unit, const double visionRange);

#ifdef ENABLE_VISUALIZATOR
      void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

    private:
      void calculateCost(const Obstacles& obstacles, const double radius);
      void calculateWeight(Vector2D<int> to);
      void findPath(Vector2D<int> from, Vector2D<int> to);

      Position calculateNearestCurvaturePoint(const Position& from, Position to, const double radius);

    private:
      Position toReal(Vector2D<int> point, double dx = 0, double dy = 0) const;
      Vector2D<int> toInt(Position point) const;

    private:
      Position to;

      static const size_t step = 20;
      static const size_t memorySize = 4000 / step;
      static const size_t maxValue = 100000;
      float weights[memorySize][memorySize];
      float costs[memorySize][memorySize];

      static const size_t areaSize = memorySize * memorySize / 2;
      /// массив координат, чтобы быстро обойти волну
      Vector2D<int> area[areaSize];

      /// конечный путь
      size_t pathSize;
      Vector2D<int> path[areaSize];
    };

  };
}