//
//File: CM_CommandDefendPoint.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandDefendPoint.h"

using namespace AICup;

CommandDefendPoint::CommandDefendPoint(Algorithm::PathFinder& finder, double x, double y):
  MoveCommand(finder) {

}

bool CommandDefendPoint::check(const model::Wizard& self) {
  return false;
}

int CommandDefendPoint::priority(const model::Wizard& self) {
  return 0;
}


void CommandDefendPoint::execute(const model::Wizard& self, Result& result) {
}