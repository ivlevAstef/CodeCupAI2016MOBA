//
//File: A_ChangeLine.h
//Author: Ivlev Alexander. Stef
//Created: 30/11/2016
//


#pragma once

#include "E_Wizard.h"
#include "model\LaneType.h"
#include "A_PathFinder.h"

namespace AICup
{
  namespace Algorithm
  {
    double calculateLinePriority(const Algorithm::PathFinder& finder, const Wizard& self, const model::LaneType lane);
    double calculateLineEnemyPriority(const Algorithm::PathFinder& finder, const Wizard& self, const model::LaneType lane);
    bool checkChangeLine(const Algorithm::PathFinder& finder, const Wizard& self, model::LaneType& lane);

    double potensialExpirience(const Wizard& self);
  };

};