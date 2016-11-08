//
//File: MyStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "Strategy.h"

class MyStrategy: public Strategy {
  void move(const model::Wizard& self, const model::World& world, const model::Game& game, model::Move& move);
};