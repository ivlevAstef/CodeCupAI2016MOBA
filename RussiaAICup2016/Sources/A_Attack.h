//
//File: A_Attack.h
//Author: Ivlev Alexander. Stef
//Created: 17/11/2016
//


#pragma once

#include "C_Vector2D.h"
#include "model\Wizard.h"
#include "E_Types.h"

namespace AICup
{
  namespace Algorithm
  {
    bool checkIntersectedTree(const Position& p1, const Position& p2, const double radius);
    bool checkIntersectedTree(const Position& p1, const Position& p2, const double radius, const long long ignoreId);

    bool isMelee(const model::Wizard& wizard, const model::LivingUnit& unit);

    /// происходит эмул€ци€ движени€, которое должно давать наилучший способ уклонени€
    bool canEscape(const model::Wizard& attacking, const model::Wizard& prey, const double bulletSpeed, const double bulletRadius);

    double timeToTurnForAttack(const model::Unit& attacked, const model::Wizard& attacking);
  };

};