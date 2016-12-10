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
#include "E_Wizard.h"
#include "T_Session.h"

using namespace AICup;


MyStrategy::MyStrategy() {
#ifdef ENABLE_VISUALIZATOR
  Visualizator::instance();
#endif
}

#ifdef ENABLE_VISUALIZATOR
void visualization(const model::Wizard& self, const Visualizator& visualizator) {
  AICup::World::instance().visualization(visualizator);
  AICup::Points::instance().visualization(visualizator);
  AICup::HypotheticalEnemies::instance().visualization(visualizator);
  AICup::InfluenceMap::instance().visualization(visualizator);
  AICup::StrategyManager::instance().visualization(self, visualizator);
}
#endif

void MyStrategy::move(const model::Wizard& modelSelf, const model::World& world, const model::Game& game, model::Move& move) {
  /// ќбновл€ю различные карты, которые не завис€т от моего геро€
  AICup::Game::instance().update(game, modelSelf);
  AICup::World::instance().update(world);
  AICup::HypotheticalEnemies::instance().update();
  AICup::InfluenceMap::instance().update(modelSelf);

  /// «апускаю обновление сессии - локальных данных, которые вли€ют на прин€тие решений что делать
  Session::instance().update(modelSelf, move);

  /// —оздаю экземпл€р своего мага, чтобы дальше с ним работать
  const auto self = Wizard(modelSelf, Session::instance().getRole());

  /// запускаю стратегию, дл€ прин€т€ решени€ что делать
  AICup::StrategyManager::instance().update(Session::instance().getStrategyType(), self, move);

#ifdef ENABLE_VISUALIZATOR
  auto& visualizator = Visualizator::instance();
  visualizator.isReverse = (self.getFaction() == model::FACTION_RENEGADES);

  visualizator.beginPre();
  visualization(self, visualizator);
  visualizator.endPre();

  visualizator.beginPost();
  visualization(self, visualizator);
  visualizator.endPost();


  visualizator.isReverse = false;
  visualizator.beginAbs();
  visualization(self, visualizator);
  visualizator.endAbs();

#endif
}
