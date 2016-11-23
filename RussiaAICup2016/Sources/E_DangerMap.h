//
//File: E_DangerMap.h
//Author: Ivlev Alexander. Stef
//Created: 23/11/2016
//

#pragma once

#include "C_Singleton.h"
#include "C_Vector2D.h"
#include "model\CircularUnit.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class DangerMapConstants {
  public:
    static const size_t step = 200;
    static const size_t memorySize = 4000 / step;
    static const int neutral = 0;

    static Position toReal(Vector2D<int> point, double dx = 0, double dy = 0);
    static Vector2D<int> toInt(Position point);
  };

  class DangerMap: public Singleton<DangerMap> {
  public:
    void update();

    const float const* getFriendsMap() const;
    const float const* getEnemiesMap() const;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
    void clean();
    void includeFriends();
    void includeEnemies();
    void includeHypotheticalEnemies();

    void includeFriend(const model::Unit& unit, const double radius, const float danger);
    void includeEnemy(const model::Unit& unit, const double radius, const float danger);

  private:
    /// положительные свои, отрицательные враги
    float friends[DangerMapConstants::memorySize][DangerMapConstants::memorySize];
    float enemies[DangerMapConstants::memorySize][DangerMapConstants::memorySize];
  };
}