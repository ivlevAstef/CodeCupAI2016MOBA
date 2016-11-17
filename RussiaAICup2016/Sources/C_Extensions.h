//
//File: Extensions.h
//Description:
//Author: Ivlev Alexander. Stef
//Created: 21:21 16/11/2016
//Copyright (c) SIA 2016. All Right Reserved.
//
#pragma once

#include "C_Vector2D.h"
#include "model/CircularUnit.h"
#include "model/Wizard.h"

namespace AICup
{
  namespace EX
  {
    inline Position pos(const model::Unit& unit) {
      return Position(unit.getX(), unit.getY());
    }

    double maxSpeed(const model::CircularUnit& obj);
    double radiusForGuaranteedHit(const model::Wizard& obj);

    std::vector<bool> availableSkills(const model::Wizard& obj);
    int minTimeForMagic(const model::Wizard& obj);

    double turnSpeed(const model::Wizard& obj);

    double timeToTurnForAttack(const model::Unit& attacked, const model::Wizard& attacking);

    double magicMissleAttack(const model::Wizard& obj);
  }
}