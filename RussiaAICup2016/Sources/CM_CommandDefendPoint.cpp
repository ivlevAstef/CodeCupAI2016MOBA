//
//File: CM_CommandDefendPoint.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandDefendPoint.h"
#include "CM_MovePriorities.h"

using namespace AICup;

CommandDefendPoint::CommandDefendPoint(Algorithm::PathFinder& finder, double, double):
  MoveCommand(finder) {

}

bool CommandDefendPoint::check(const model::Wizard&) {
  return false;
}

void CommandDefendPoint::execute(const model::Wizard&, Result&) {
  //result.priority = MovePriorities::defendPoint(self, );
}