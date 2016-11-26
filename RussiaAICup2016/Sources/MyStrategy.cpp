//
//File: MyStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "MyStrategy.h"
#include "E_World.h"
#include "E_InfluenceMap.h"
#include "E_Game.h"
#include "E_Points.h"
#include "C_Logger.h"
#include "E_HypotheticalEnemies.h"
#include "S_StrategyManager.h"
#include "C_Extensions.h"
#include "C_Logger.h"


using namespace model;
using namespace AICup;


MyStrategy::MyStrategy() {
#ifdef ENABLE_VISUALIZATOR
  Visualizator::instance();
#endif
}

#ifdef ENABLE_VISUALIZATOR
void visualization(const Visualizator& visualizator) {
  AICup::World::instance().visualization(visualizator);
  AICup::Points::instance().visualization(visualizator);
  AICup::HypotheticalEnemies::instance().visualization(visualizator);

  AICup::InfluenceMap::instance().visualization(visualizator);

  AICup::StrategyManager::instance().visualization(visualizator);
}
#endif

void MyStrategy::move(const model::Wizard& self, const model::World& world, const model::Game& game, model::Move& move) {
  AICup::Game::instance().update(game, self);
  AICup::World::instance().update(world);
  AICup::HypotheticalEnemies::instance().update();
  AICup::InfluenceMap::instance().update();
  AICup::StrategyManager::instance().update(self, move);

#ifdef ENABLE_VISUALIZATOR
  auto& visualizator = Visualizator::instance();
  visualizator.isReverse = (self.getFaction() == model::FACTION_RENEGADES);

  visualizator.beginPre();
  visualization(visualizator);
  visualizator.endPre();

  visualizator.beginPost();
  visualization(visualizator);
  visualizator.endPost();

  visualizator.beginAbs();
  visualization(visualizator);
  visualizator.endAbs();

#endif
}
