//
//File: CM_CommandMoveToLine.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToLine.h"

using namespace AICup;

CommandMoveToLine::CommandMoveToLine(model::LineType line) {

}

bool AICup::CommandMoveToLine::execute() {
  return false;
}

const std::vector<CommandPtr>& AICup::CommandMoveToLine::neededCommands(const CommandFabric& fabric) {
  return needCommands;
}
