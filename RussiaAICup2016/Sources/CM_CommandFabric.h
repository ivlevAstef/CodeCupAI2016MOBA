//
//File: CM_CommandFabric.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include "CM_Command.h"
#include "model\LaneType.h"
#include <initializer_list>

namespace AICup
{
  class CommandFabric {
  public:
#pragma mark - перемещение
    /// даржаться от точки на определенном расстоянии
    CommandPtr keepDistance(const double x, const double y, const double minDistance, const double maxDistance) const;

    /// переместиться в точку
    CommandPtr moveToPoint(const double x, const double y) const;

    /// переместиться на линию
    CommandPtr moveToLine(const model::LaneType line) const;

    /// следовать за юнитом
    CommandPtr follow(const long long unitId, const double minDistance = 0, const double maxDistance = 10000) const;

    /// подойти за получением опыта
    CommandPtr moveGetExpirience() const;

    /// подойти чтобы взять руну
    CommandPtr moveToBonus() const;

    /// обследование карты - посещение мест где был давно
    CommandPtr observeMap() const;

#pragma mark - атака
    /// атаковать врага
    CommandPtr attack(const long long enemyId) const;

    /// пул нейтральных крипов
    CommandPtr pool(const long long neutralUnitId) const;


#pragma mark - защита
    /// защищать позицию
    CommandPtr defend(const double x, const double y) const;

    /// держаться на безопасном расстоянии от юнита
    CommandPtr avoidEnemy(const long long unitId) const;

    /// держаться на безопасном расстоянии от всех врагов вокруге
    CommandPtr avoidAround() const;

  };
}
