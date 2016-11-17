//
//File: CM_CommandObserveMap.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandObserveMap.h"

using namespace AICup;

CommandObserveMap::CommandObserveMap() {

}

bool CommandObserveMap::check(const model::Wizard& self) {
  return false;
}

int CommandObserveMap::priority(const model::Wizard& self) {
  return 0;
}

void CommandObserveMap::execute(const model::Wizard& self, Result& result) {
}