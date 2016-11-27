//
//File: CM_AttackPriorities.h
//Author: Ivlev Alexander. Stef
//Created: 25/11/2016
//

#pragma once

#include "E_Wizard.h"
#include "model\Minion.h"
#include "model\LaneType.h"
#include "C_Vector2D.h"


namespace AICup
{

  // ќбъедин€ю в один файл, чтобы легче было настраивать приоритеты
  namespace AttackPriorities {
    int attackEnemy(const Wizard& self, const model::LivingUnit& enemy);

    int pool(const Wizard& self, const model::Minion& neutral);

  };
}

