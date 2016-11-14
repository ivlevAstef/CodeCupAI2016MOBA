#include "CM_TestFollowStrategy.h"
#include "E_World.h"

using namespace AICup;

TestFollowStrategy::TestFollowStrategy(const CommandFabric& fabric) : CommandStategy(fabric) {
  for (const auto& wizard : World::instance().model().getWizards()) {
    if (!wizard.isMe()) {
      moveCommand = fabric.follow(wizard.getId(), 200, 235);
      break;
    }
  }
}

void TestFollowStrategy::update(const model::Wizard& self, model::Move& move) {
  if (moveCommand->check(self)) {
    moveCommand->execute(self, move);
  }
}

#ifdef ENABLE_VISUALIZATOR
void TestFollowStrategy::visualization(const Visualizator& visualizator) const {
  moveCommand->visualization(visualizator);
}

#endif