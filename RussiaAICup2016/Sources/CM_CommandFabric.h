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
    /// переместиться в точку
    CommandPtr moveToPoint(double x, double y);

    /// переместиться на линию
    CommandPtr moveToLine(model::LineType line);

    /// атаковать врага
    CommandPtr attack(long long enemyId);

    /// защищать позицию
    CommandPtr defend(double x, double y);

    /// пул нейтральных крипов
    //CommandPtr pool(double x, double y);

    /// Для возможности одновременного исполнения команд
    CommandPtr sequence(std::initializer_list<CommandPtr> commands);
  };
}
