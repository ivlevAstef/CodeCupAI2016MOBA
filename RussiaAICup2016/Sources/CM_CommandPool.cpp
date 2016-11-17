//
//File: CM_CommandPool.cpp
//Author: Ivlev Alexander. Stef
//Created: 08/11/2016
//


#include "CM_CommandPool.h"

using namespace AICup;

CommandPool::CommandPool(long long neutralUnitId) {

}

bool CommandPool::check(const model::Wizard& self) {
  return false;
}

int CommandPool::priority(const model::Wizard& self) {
  return 0;
}

void CommandPool::execute(const model::Wizard& self, Result& result) {
}