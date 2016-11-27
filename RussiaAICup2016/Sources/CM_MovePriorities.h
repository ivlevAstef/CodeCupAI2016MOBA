//
//File: CM_MovePriorities.h
//Author: Ivlev Alexander. Stef
//Created: 25/11/2016
//

#pragma once

#include "E_Wizard.h"
#include "model\LaneType.h"
#include "C_Vector2D.h"


namespace AICup
{

  // ќбъедин€ю в один файл, чтобы легче было настраивать приоритеты
  namespace MovePriorities {
    int avoidEnemy(const Wizard& self, const model::CircularUnit& enemy);

    int defendPoint(const Wizard& self, const Position& point);

    int follow(const Wizard& self, const model::LivingUnit& unit);
    int getExpirience(const Wizard& self, const model::LivingUnit& unit);
    int keepDistance(const Wizard& self, const Position point, const double min, const double max);

    int moveToBonus(const Wizard& self, const Position& bonusPos);

    int moveToLine(const Wizard& self, const model::LaneType& lane);

    int moveToPoint(const Wizard& self, const Position& point);

    int observeMap(const Wizard& self);

  };
}

