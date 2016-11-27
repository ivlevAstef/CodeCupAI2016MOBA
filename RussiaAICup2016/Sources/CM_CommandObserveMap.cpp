//
//File: CM_CommandObserveMap.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandObserveMap.h"
#include "CM_MovePriorities.h"

using namespace AICup;

CommandObserveMap::CommandObserveMap(Algorithm::PathFinder& finder): MoveCommand(finder) {

}

bool CommandObserveMap::check(const Wizard&) {
  return false;
}

void CommandObserveMap::execute(const Wizard&, Result&) {
}