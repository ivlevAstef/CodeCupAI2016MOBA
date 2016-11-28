//
//File: CM_CommandPool.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandPool.h"
#include "CM_AttackPriorities.h"

using namespace AICup;

CommandPool::CommandPool(long long) {

}

bool CommandPool::check(const Wizard&) {
  return false;
}

void CommandPool::execute(const Wizard&, Result&) {
}

double CommandPool::priority(const Wizard& self) {
  return 0;
}