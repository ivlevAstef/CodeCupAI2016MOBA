//
//File: CM_MovePriorities.h
//Author: Ivlev Alexander. Stef
//Created: 25/11/2016
//

#pragma once

#include "model\Wizard.h"
#include "model\LaneType.h"
#include "C_Vector2D.h"


namespace AICup
{

  // ќбъедин€ю в один файл, чтобы легче было настраивать приоритеты
  namespace MovePriorities {
    int avoidEnemy(const model::Wizard& self, const model::LivingUnit& enemy);

    int defendPoint(const model::Wizard& self, const Position& point);

    int follow(const model::Wizard& self, const model::LivingUnit& unit);
    int getExpirience(const model::Wizard& self, const model::LivingUnit& unit);
    int keepDistance(const model::Wizard& self, const Position point, const double min, const double max);

    int moveToBonus(const model::Wizard& self, const Position& bonusPos);

    int moveToLine(const model::Wizard& self, const model::LaneType& lane);

    int moveToPoint(const model::Wizard& self, const Position& point);

    int observeMap(const model::Wizard& self);

  };
}

