#include "S_TestFollowStrategy.h"
#include "E_World.h"

#include "R_RoundOne.h"

using namespace AICup;

TestFollowStrategy::TestFollowStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder):
  CommandStrategy(fabric, pathFinder, std::make_shared<RoundOneRole>(), std::make_shared<RoundOneSkillBuild>()) {
  for (const auto& wizard : World::instance().model().getWizards()) {
    if (!wizard.isMe()) {
      moveCommands.push_back(fabric.follow(wizard.getId(), 200, 235));
      break;
    }
  }
}

void TestFollowStrategy::update(const model::Wizard& model, model::Move& move) {
  const auto& self = CommandStrategy::preUpdate(model, move);
  CommandStrategy::update(self, move);
}