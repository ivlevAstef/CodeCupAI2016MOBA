//
//File: A_Attack.h
//Author: Ivlev Alexander. Stef
//Created: 17/11/2016
//


#pragma once

#include "C_Vector2D.h"
#include "model\Wizard.h"
#include "model\Building.h"
#include "E_Types.h"
#include "E_Bullet.h"

namespace AICup
{
  namespace Algorithm
  {
    bool checkIntersectedTree(const Position& p1, const Position& p2, const double radius);
    bool checkIntersectedTree(const Position& p1, const Position& p2, const double radius, const long long ignoreId);

    bool isMelee(const model::Wizard& wizard, const model::LivingUnit& unit);

    ///расчитывает вектор по которому можно уклониться, если он нулевой - уклонение невозможно
    bool canDodge(const model::Wizard& prey, const Vector desiredDir, const Bullet bullet);
    bool canDodge(const model::Wizard& prey, const Position& preyBeginPos, const Vector desiredDir, const Bullet bullet);
    Vector dodge(const model::Wizard& prey, const Vector desiredDir, const Bullet bullet, int& turnSign);

    double timeToTurnForAttack(const model::Unit& attacked, const model::Wizard& attacking);
    double timeToTurn(const model::Wizard& wizard, const double angle);

    bool canAttackMMOrMelee(const model::Wizard& self, const model::LivingUnit& unit);

    bool isImmortal(const model::Building& building);
  };

};