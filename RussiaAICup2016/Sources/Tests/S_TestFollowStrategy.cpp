#include "S_TestFollowStrategy.h"
#include "E_World.h"

using namespace AICup;

TestFollowStrategy::TestFollowStrategy(const CommandFabric& fabric) : CommandStategy(fabric) {
  for (const auto& wizard : World::instance().model().getWizards()) {
    if (!wizard.isMe()) {
      moveCommands.push_back(fabric.follow(wizard.getId(), 200, 235));
      break;
    }
  }
}