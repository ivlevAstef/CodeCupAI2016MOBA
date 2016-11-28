//
//File: CM_CommandDefendPoint.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandDefendPoint.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"

using namespace AICup;

CommandDefendPoint::CommandDefendPoint(Algorithm::PathFinder& finder, double, double):
  MoveCommand(finder) {

}

bool CommandDefendPoint::check(const Wizard&) {
  return false;
}

double CommandDefendPoint::priority(const Wizard&) {
  return 0;
}

void CommandDefendPoint::execute(const Wizard&, Result&) {
  //result.priority = MovePriorities::defendPoint(self, );
}