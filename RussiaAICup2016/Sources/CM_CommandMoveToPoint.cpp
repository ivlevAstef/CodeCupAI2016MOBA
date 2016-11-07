//
//File: CM_CommandMoveToPoint.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandMoveToPoint.h"

using namespace AICup;

CommandMoveToPoint::CommandMoveToPoint(double x, double y) {

}

bool AICup::CommandMoveToPoint::execute() {
  return false;
}

const std::vector<CommandPtr>& AICup::CommandMoveToPoint::neededCommands(const CommandFabric& fabric) {
  return needCommands;
}
