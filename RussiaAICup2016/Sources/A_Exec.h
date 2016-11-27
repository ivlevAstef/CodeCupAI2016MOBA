//
//File: A_Exec.h
//Author: Ivlev Alexander. Stef
//Created: 17/11/2016
//


#pragma once

#include "C_Vector2D.h"
#include "E_Wizard.h"
#include "model\Move.h"
#include "E_Types.h"

namespace AICup
{
  namespace Algorithm
  {
    bool execMove(const Wizard& self, const TurnStyle style, const Vector& direction, const double speedLimit, model::Move& move);

    bool execAttack(const Wizard& self, const model::ActionType action, const model::LivingUnit& unit, model::Move& move);
  };

};