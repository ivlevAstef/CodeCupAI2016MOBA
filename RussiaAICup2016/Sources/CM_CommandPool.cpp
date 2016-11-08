//
//File: CM_CommandPool.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandPool.h"

using namespace AICup;

CommandPool::CommandPool(long long neutralUnitId) {

}

bool AICup::CommandPool::execute(const model::Wizard& self, model::Move& move) {
  return false;
}

const std::vector<CommandPtr>& AICup::CommandPool::neededCommands(const CommandFabric& fabric) {
  return needCommands;
}
