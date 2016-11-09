//
//File: CM_CommandFollow.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandFollow.h"

using namespace AICup;

CommandFollow::CommandFollow(long long unitId) {

}


bool CommandFollow::check(const model::Wizard& self, model::Move& move) {
  return false;
}

void CommandFollow::execute(const model::Wizard& self, model::Move& move) {
}