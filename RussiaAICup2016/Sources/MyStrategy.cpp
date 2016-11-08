//
//File: MyStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "MyStrategy.h"
#include "E_World.h"
#include "E_Graph.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif


using namespace model;

void MyStrategy::move(const Wizard& self, const World& world, const Game& game, Move& move) {
  AICup::World::instance().update(world);
  AICup::Graph::instance().update();

  move.setSpeed(-game.getWizardBackwardSpeed());
  move.setAction(ACTION_STAFF);

#ifdef ENABLE_VISUALIZATOR
  auto& visualizator = AICup::Visualizator::instance();
#endif
}
