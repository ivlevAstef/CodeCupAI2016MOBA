//
//File: A_Exec.h
//Author: Ivlev Alexander. Stef
//Created: 17/11/2016
//


#pragma once

#include "C_Vector2D.h"
#include "model\Wizard.h"
#include "model\Move.h"
#include "E_Types.h"

namespace AICup
{
  namespace Algorithm
  {
    bool execMove(const model::Wizard& self, const TurnStyle style, const Vector& direction, model::Move& move);

    bool execAttack(const model::Wizard& self, const model::ActionType action, const model::LivingUnit& unit, model::Move& move);
  };

};