//
//File: CM_CommandFabric.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#pragma once

#include "CM_Command.h"
#include "model\LineType.h"
#include <initializer_list>

namespace AICup
{
  class CommandFabric {
  public:
#pragma mark - перемещение
    /// переместиться в точку
    CommandPtr moveToPoint(double x, double y) const;

    /// переместиться на линию
    CommandPtr moveToLine(model::LineType line) const;

    /// следовать за юнитом
    CommandPtr follow(long long unitId) const;

    /// обследование карты - посещение мест где был давно
    CommandPtr observeMap() const;

#pragma mark - атака
    /// атаковать врага
    CommandPtr attack(long long enemyId) const;

    /// пул нейтральных крипов
    CommandPtr pool(long long neutralUnitId) const;


#pragma mark - защита
    /// защищать позицию
    CommandPtr defend(double x, double y) const;

  };
}
