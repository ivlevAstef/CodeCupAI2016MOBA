//
//File: CM_CommandObserveMap.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandObserveMap.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"

using namespace AICup;

CommandObserveMap::CommandObserveMap() {

}

bool CommandObserveMap::check(const Wizard&) {
  return false;
}

double CommandObserveMap::priority(const Wizard&) {
  return 0;
}

void CommandObserveMap::execute(const Wizard&, Result&) {
}