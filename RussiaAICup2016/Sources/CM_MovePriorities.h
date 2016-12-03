//
//File: CM_MovePriorities.h
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
  namespace MovePriorities {
    double pushOffMinion(const Wizard& self, const model::Minion& minion);

    double avoidBuild(const Wizard& self, const model::Building& build);
    double avoidMinion(const Wizard& self, const model::Minion& minion);
    double avoidWizard(const Wizard& self, const model::Wizard& wizard);
    double avoidProjectile(const Wizard& self, const model::Projectile& projectile);

    double defendPoint(const Wizard& self, const Position& point);

    double attackFollow(const Wizard& self, const model::Wizard& wizard);
    double follow(const Wizard& self, const model::LivingUnit& unit);
    double getExpirience(const Wizard& self, const model::LivingUnit& unit);
    double keepDistance(const Wizard& self, const Position point, const double min, const double max);

    double moveToBonus(const Wizard& self, const Position& bonusPos);

    double moveToLine(const Wizard& self, const model::LaneType& lane);

    double moveToPoint(const Wizard& self, const Position& point);

    double observeMap(const Wizard& self);

  };
}

