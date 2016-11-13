#include "CM_TestFollow.h"
#include "E_World.h"

using namespace AICup;

TestFollow::TestFollow(const CommandFabric& fabric) : CommandStategy(fabric) {
  for (const auto& wizard : World::instance().model().getWizards()) {
    if (!wizard.isMe()) {
      moveCommand = fabric.follow(wizard.getId(), 200, 235);
      break;
    }
  }
}

void TestFollow::update(const model::Wizard& self, model::Move& move) {
  if (moveCommand->check(self, move)) {
    moveCommand->execute(self, move);
  }
}

void TestFollow::visualization(const Visualizator& visualizator) const {
  moveCommand->visualization(visualizator);
}
