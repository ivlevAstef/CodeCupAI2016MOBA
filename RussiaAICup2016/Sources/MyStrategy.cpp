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

  if (AICup::World::model().getTickIndex() > 900) {
    AICup::InfluenceMap::instance().visualization(visualizator);
  }

  AICup::StrategyManager::instance().visualization(self, visualizator);
}
#endif

void MyStrategy::move(const model::Wizard& modelSelf, const model::World& world, const model::Game& game, model::Move& move) {
  static std::shared_ptr<AICup::Wizard> self = std::make_shared<Wizard>(modelSelf);
  /// —оздаю экземпл€р своего мага, чтобы дальше с ним работать
  const auto newSelf = std::make_shared<Wizard>(modelSelf);
  newSelf->moveInfoFrom(*self);
  self = newSelf;

  AICup::Game::instance().update(game, modelSelf);
  AICup::World::instance().update(world);
  /// —читывает сообщени€, мен€ет внутренние атрибуты, короче много чего делает...
  self->update(move);

  AICup::HypotheticalEnemies::instance().update();
  AICup::InfluenceMap::instance().update(*self);
  AICup::StrategyManager::instance().update(*self, move);

#ifdef ENABLE_VISUALIZATOR
  auto& visualizator = Visualizator::instance();
  visualizator.isReverse = (self->getFaction() == model::FACTION_RENEGADES);

  visualizator.beginPre();
  visualization(*self, visualizator);
  visualizator.endPre();

  visualizator.beginPost();
  visualization(*self, visualizator);
  visualizator.endPost();


  visualizator.isReverse = false;
  visualizator.beginAbs();
  visualization(*self, visualizator);
  visualizator.endAbs();

#endif
}
