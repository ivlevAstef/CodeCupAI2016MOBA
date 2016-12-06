//
//File: E_InfluenceMap.h
//Author: Ivlev Alexander. Stef
//Created: 23/11/2016
//

#pragma once

#include "C_Singleton.h"
#include "C_Vector2D.h"
#include "model\CircularUnit.h"
#include "model\LaneType.h"
#include "model\Wizard.h"
#include <vector>

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif

namespace AICup
{
  class InfluenceMapConstants {
  public:
    static const size_t step = 50;
    static const size_t memorySize = 4000 / step;
    static const int neutral = 0;

    static Position toReal(Vector2D<int> point, double dx = 0, double dy = 0);
    static Vector2D<int> toInt(Position point);
  };

  class InfluenceMap: public Singleton<InfluenceMap> {
  public:
    InfluenceMap();

    void update(const model::Wizard& self);

    const float* const getFriendsMap() const;
    const float* const getEnemiesMap() const;

    /// Возращает линию фронта на указанной линии
    /// смещение может быть отрицательным (ближе к врагам) и положительным (ближе к своим)
    /// Линия фронта - точка где находиться край своих (юнитов, зданий). за частую он совпадает с краем врагов, но не обязательно
    Position getForeFront(const model::LaneType lane, const float offset = 0) const;
    /// Возращает направление линии фронта. всегда в сторону противников
    Vector getForeDirection(const model::LaneType lane, const Position foreFront) const;

    /// положительный наших больше, отрицательный наших меньше
    float getLineStrength(const model::LaneType lane) const;

    float getStrength(const Position center, double radius) const;

#ifdef ENABLE_VISUALIZATOR
    void visualization(const Visualizator& visualizator) const;
#endif // ENABLE_VISUALIZATOR

  private:
    void updateLinePosition(const model::Wizard& self);

    void clean();
    void includeFriends();
    void includeEnemies();
    void includeHypotheticalEnemies();

    void includeFriend(const model::Unit& unit, const double radius, const float danger);
    void includeEnemy(const model::Unit& unit, const double radius, const float danger);

    const std::vector<Position>& getLinePoints(const model::LaneType lane) const;
    Position calculateForeFront(const model::Wizard& self, const model::LaneType lane) const;
    bool isFriendZone(int& x, int& y) const;
    float zonePriority(const int x, const int y) const;
    float zonePriorityAndPoint(int& x, int& y) const;
    Position pointToForeFront(const model::Wizard& self, const int x, const int y, const std::vector<Position>& line) const;

  private:
    int lastCalculateTick;

    Position topForeFront;
    Position middleForeFront;
    Position bottomForeFront;

    /// положительные свои, отрицательные враги
    float friends[InfluenceMapConstants::memorySize][InfluenceMapConstants::memorySize];
    float enemies[InfluenceMapConstants::memorySize][InfluenceMapConstants::memorySize];
  };
}