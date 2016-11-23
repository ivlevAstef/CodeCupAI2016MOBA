//
//File: CM_CommandObserveMap.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandObserveMap.h"

using namespace AICup;

CommandObserveMap::CommandObserveMap(Algorithm::PathFinder& finder): MoveCommand(finder) {

}

bool CommandObserveMap::check(const model::Wizard&) {
  return false;
}

int CommandObserveMap::priority(const model::Wizard&) {
  return 0;
}

void CommandObserveMap::execute(const model::Wizard&, Result&) {
}