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
    /// да прост€т мен€ другие программист, за эти 4! функции (а не одну...)
    bool canSideForwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius);
    bool canSideBackwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius);
    bool canBackForwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius);
    bool canBackBackwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius);

    double timeToTurnForAttack(const model::Unit& attacked, const model::Wizard& attacking);
    double timeToTurn(const model::Wizard& wizard, const double angle);
  };

};