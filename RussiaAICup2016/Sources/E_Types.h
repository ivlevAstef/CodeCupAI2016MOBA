//
//File: E_Types.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "model\CircularUnit.h"
#include "C_Vector2D.h"
#include <vector>

namespace AICup
{
  typedef std::vector<model::CircularUnit> Obstacles;
  typedef std::vector<Obstacles> ObstaclesGroups;
  typedef std::vector<Position> Path;
};