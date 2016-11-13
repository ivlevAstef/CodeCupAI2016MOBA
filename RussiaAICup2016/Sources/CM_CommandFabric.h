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
    /// переместиться в точку
    CommandPtr moveToPoint(const double x, const double y) const;

    /// переместиться на линию
    CommandPtr moveToLine(const model::LaneType line) const;

    /// следовать за юнитом
    CommandPtr follow(const long long unitId, const double minDistance = 0, const double maxDistance = 10000) const;

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

  };
}
