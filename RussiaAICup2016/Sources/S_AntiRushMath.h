//
//File: R_AntiRushMath.h
//Author: Ivlev Alexander. Stef
//Created: 12/12/2016
//

#pragma once

#include "C_Vector2D.h"
#include "model/LaneType.h"

namespace AICup
{
  namespace AntiRushMath {
    Position laneToPosition(model::LaneType lane);
  };
}