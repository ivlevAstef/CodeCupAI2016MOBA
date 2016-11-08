//
//File: CM_CommandDefendPoint.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandDefendPoint.h"

using namespace AICup;

CommandDefendPoint::CommandDefendPoint(double x, double y) {

}

bool AICup::CommandDefendPoint::execute(const model::Wizard& self, model::Move& move) {
  return false;
}

const std::vector<CommandPtr>& AICup::CommandDefendPoint::neededCommands(const CommandFabric& fabric) {
  return needCommands;
}
