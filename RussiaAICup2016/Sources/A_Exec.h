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
    bool execMove(const Wizard& self, const Vector& turnDirection, const Vector& direction, model::Move& move);

    bool execAroundMove(const Wizard& self, model::Move& move);

    bool execAttack(const Wizard& self, const model::ActionType action, const model::LivingUnit& unit, model::Move& move);

    bool execCast(const Wizard& self, const model::ActionType action, const model::LivingUnit& unit, model::Move& move);
  };

};