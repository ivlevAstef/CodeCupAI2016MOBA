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

    /// происходит эмуляция движения, на простой способ - уклонение в бок
    bool canSideForwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius);
    bool canSideBackwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius);

    ///расчитывает вектор по которому можно уклониться, если он нулевой - уклонение невозможно
    Vector dodge(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius, TurnStyle& turnStyle);

    double timeToTurnForAttack(const model::Unit& attacked, const model::Wizard& attacking);
    double timeToTurn(const model::Wizard& wizard, const double angle);
  };

};