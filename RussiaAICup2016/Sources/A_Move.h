//
//File: A_Move.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "C_Vector2D.h"
#include "model\Wizard.h"
#include "E_Types.h"
#include <vector>

namespace AICup
{
  namespace Algorithm
  {
    Vector maxSpeed(const model::Wizard& wizard, const double wizardAngle, const Vector& direction);

    /// ¬озращает вектор движени€, учитыва€ преп€тстви€
    Vector move(const model::CircularUnit& unit, const Position& pos, const ObstaclesGroups& obstacles, const double range);
  };

};