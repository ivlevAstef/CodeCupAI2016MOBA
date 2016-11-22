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
    class PathConstants {
    public:
      static const size_t step = 25;
      static const size_t memorySize = 4000 / step;
      static const size_t maxValue = 100000;

      static const size_t maxPath = memorySize * memorySize / 2;

      static Position toReal(Vector2D<int> point, double dx = 0, double dy = 0);
      static Vector2D<int> toInt(Position point);
    };

    class Path {
    private:
      Path(Position from, Position to, const double radius, const Obstacles&);

    public:
      Position calculateNearestCurvaturePoint(const double visionRange) const;

      double getLength() const { return length; }
      const Position& getFrom() const { return from; }
      const Position& getTo() const { return to; }

      /// возращает список удаленных
      Obstacles removeObstacles(Obstacles& obstacles) const;

#ifdef ENABLE_VISUALIZATOR
      void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

    private:
      bool checkRemoved(const Obstacles& obstacles, const model::LivingUnit* tree) const;
    private:
      friend class PathFinder;

      const Position from;
      const Position to;
      const double radius;

      /// конечный путь, но он перевернут - to ---> from
      size_t count;
      Position path[PathConstants::maxPath];

      double length;

      const Obstacles& obstacles;
    };




    class PathFinder {
    public:
      PathFinder();

      ///Возращает точку куда надо идти
      void calculate(const model::CircularUnit& unit);

      void calculatePath(const Position& to, Path** path) const;

#ifdef ENABLE_VISUALIZATOR
      void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

    private:
      void calculateCost(const Obstacles& obstacles, const double radius);
      void calculateWeight(Vector2D<int> to);
      void calculatePath(Path& path) const;

    private:
      void clean();

    private:
      Position from;
      double radius;

      float weights[PathConstants::memorySize][PathConstants::memorySize];
      float costs[PathConstants::memorySize][PathConstants::memorySize];

      Obstacles obstacles;
    };

  };
}