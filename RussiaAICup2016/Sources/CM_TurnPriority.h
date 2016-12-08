//
//File: CM_TurnPrioritiy.h
//Author: Ivlev Alexander. Stef
//Created: 25/11/2016
//

#pragma once

#include "E_Wizard.h"
#include "model\LaneType.h"
#include "model\Building.h"
#include "model\Minion.h"
#include "model\Projectile.h"
#include "C_Vector2D.h"


namespace AICup
{

  // ќбъедин€ю в один файл, чтобы легче было настраивать приоритеты
  class TurnPriority
  {
  public:
    static const int avoidBuild = 5;
    static const int avoidMinion = 5;
    static const int avoidWizard = 10;
    static const int avoidProjectile = 100;

    static const int defendPoint = 1;

    static const int follow = 1;
    static const int getExpirience = 1;

    static const int moveToBonus = 1;

    static const int moveToLine = 1;

    static const int moveToPoint = 1;

  };
}

