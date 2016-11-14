//
//File: E_Game.cpp
//Author: Ivlev Alexander. Stef
//Created: 09/11/2016
//

#include "E_Game.h"

using namespace AICup;

void Game::update(const model::Game& Game) {
  modelGame = &Game;
}

const model::Game& Game::model() const {
  return *modelGame;
}