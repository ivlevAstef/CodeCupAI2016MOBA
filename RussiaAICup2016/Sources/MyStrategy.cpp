//
//File: MyStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "MyStrategy.h"
#include "E_World.h"
#include "E_Game.h"
#include "E_Points.h"
#include "C_Logger.h"
#include "S_StrategyManager.h"


using namespace model;
using namespace AICup;


MyStrategy::MyStrategy() {
#ifdef ENABLE_VISUALIZATOR
  Visualizator::instance();
#endif
}

void MyStrategy::move(const model::Wizard& self, const model::World& world, const model::Game& game, model::Move& move) {
  AICup::Game::instance().update(game);
  AICup::World::instance().update(world);

  AICup::StrategyManager::instance().update(self, move);

#ifdef ENABLE_VISUALIZATOR
  auto& visualizator = Visualizator::instance();

  visualizator.isReverse = (self.getFaction() == model::FACTION_RENEGADES);

  visualizator.beginPre();

  AICup::World::instance().visualization(visualizator);
  AICup::Points::instance().visualization(visualizator);
  AICup::StrategyManager::instance().visualization(visualizator);

  visualizator.endPre();

  visualizator.beginPost();

  AICup::StrategyManager::instance().visualization(visualizator);

  visualizator.endPost();

  visualizator.beginAbs();

  AICup::StrategyManager::instance().visualization(visualizator);

  visualizator.endAbs();

#endif
}
