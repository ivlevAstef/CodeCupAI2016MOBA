//
//File: CM_CommandAttackEnemy.cpp
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#include "CM_CommandAttackEnemy.h"

using namespace AICup;

CommandAttackEnemy::CommandAttackEnemy(long long enemyId) {

}

bool AICup::CommandAttackEnemy::execute() {
  return false;
}

const std::vector<CommandPtr>& AICup::CommandAttackEnemy::neededCommands(const CommandFabric& fabric) {
  return needCommands;
}
