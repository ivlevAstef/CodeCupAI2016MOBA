//
//File: MyStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "MyStrategy.h"

using namespace model;

void MyStrategy::move(const Wizard& self, const World& world, const Game& game, Move& move) {
  move.setSpeed(-game.getWizardBackwardSpeed());
  move.setAction(ACTION_STAFF);
}
