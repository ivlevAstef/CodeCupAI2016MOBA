//
//File: CM_CommandObserveMap.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandObserveMap.h"

using namespace AICup;

CommandObserveMap::CommandObserveMap() {

}

bool AICup::CommandObserveMap::execute() {
  return false;
}

const std::vector<CommandPtr>& AICup::CommandObserveMap::neededCommands(const CommandFabric& fabric) {
  return needCommands;
}
