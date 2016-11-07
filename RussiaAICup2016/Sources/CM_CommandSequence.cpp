//
//File: CM_CommandSequence.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandSequence.h"

using namespace AICup;

CommandSequence::CommandSequence(std::initializer_list<CommandPtr> commands) {

}

bool AICup::CommandSequence::execute() {
  return false;
}

const std::vector<CommandPtr>& AICup::CommandSequence::neededCommands(const CommandFabric& fabric) {
  return needCommands;
}
