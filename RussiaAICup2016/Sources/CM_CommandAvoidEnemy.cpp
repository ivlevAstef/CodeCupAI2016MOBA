//
//File: CM_CommandAvoidEnemy.cpp
//Author: Ivlev Alexander. Stef
//Created: 14/11/2016
//


#include "CM_CommandAvoidEnemy.h"

using namespace AICup;

CommandAvoidEnemy::CommandAvoidEnemy(const long long enemyId): enemyId(enemyId) {

}

bool CommandAvoidEnemy::check(const model::Wizard& self) {
  return false;
}

int CommandAvoidEnemy::priority(const model::Wizard& self) {
  return 0;
}


void CommandAvoidEnemy::execute(const model::Wizard& self, model::Move& move) {
}