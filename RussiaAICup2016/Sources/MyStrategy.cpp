//
//File: MyStrategy.h
//Author: Ivlev Alexander. Stef
//Created: 07/11/2016
//

#include "MyStrategy.h"
#include "E_World.h"
#include "E_Graph.h"
#include "C_Logger.h"

#ifdef ENABLE_VISUALIZATOR
#include "Visualizator.h"
#endif


using namespace model;
using namespace AICup;

MyStrategy::MyStrategy() {
  Visualizator::instance();
}

void MyStrategy::move(const model::Wizard& self, const model::World& world, const model::Game& game, model::Move& move) {
  AICup::World::instance().update(world);
  AICup::Graph::instance().update();

  move.setSpeed(-game.getWizardBackwardSpeed());
  move.setAction(ACTION_STAFF);

#ifdef ENABLE_VISUALIZATOR
  auto& visualizator = Visualizator::instance();
  visualizator.setWindowCenter(self.getX(), self.getY());

  visualizator.beginPost();

  visualizator.fillCircle(self.getX(), self.getY(), self.getRadius() * 2, 0xff0000);

  visualizator.endPost();

#endif
}
