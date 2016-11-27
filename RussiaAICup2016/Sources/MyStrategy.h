//
//File: MyStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//


#pragma once

#include "Strategy.h"
#include "E_Wizard.h"

class MyStrategy: public Strategy {
public:
  MyStrategy();

  void move(const model::Wizard& self, const model::World& world, const model::Game& game, model::Move& move);
};