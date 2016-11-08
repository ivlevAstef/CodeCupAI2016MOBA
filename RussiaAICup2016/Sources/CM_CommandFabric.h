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

#pragma mark - перемещение
    /// переместиться в точку
    CommandPtr moveToPoint(double x, double y);

    /// переместиться на линию
    CommandPtr moveToLine(model::LineType line);

    /// следовать за юнитом
    CommandPtr follow(long long unitId);

    /// обследование карты - посещение мест где был давно
    CommandPtr observeMap();

#pragma mark - атака
    /// атаковать врага
    CommandPtr attack(long long enemyId);

    /// пул нейтральных крипов
    CommandPtr pool(long long neutralUnitId);


#pragma mark - защита
    /// защищать позицию
    CommandPtr defend(double x, double y);

  };
}
