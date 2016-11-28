//
//File: E_Types.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "model\LivingUnit.h"
#include "C_Vector2D.h"
#include <vector>

namespace AICup
{
  typedef std::vector<const model::LivingUnit*> Obstacles;
  typedef std::vector<Obstacles> ObstaclesGroups;

  enum class TurnStyle {
    NO_TURN,
    TURN,
    BACK_TURN,
    SIDE_TURN
  };
};