//
//File: A_WinPredictor.h
//Author: Ivlev Alexander. Stef
//Created: 06/12/2016
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
    /// предсказывает насколько большой шанс выжить и убить мага в драке если мы пойдем в атаку
    /// возращает 1 если шансы 100%, -1 если нас точно убьют
    /// x,y позиция врага
    double changeOfWinning(const model::Wizard& self, double friendMult, double enemyMult, const double x = -1, const double y = -1, double strengthMult = 1);

    double symmetrialChangeOfWinning(const model::Wizard& self, const model::Wizard& enemy, double friendMult, double enemyMult);
  };

};