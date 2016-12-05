#include "S_TestDodgeStrategy.h"
#include "E_World.h"

using namespace AICup;

TestDodgeStrategy::TestDodgeStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder) :
  CommandStrategy(fabric, pathFinder) {
}

void TestDodgeStrategy::update(const Wizard& self, model::Move& move) {
  CommandStrategy::clear();

  /*const auto moveCommand = fabric.moveToPoint(1200, 1200);
  if (moveCommand->check(self)) {
    moveCommands.push_back(moveCommand);
  }*/

  const auto moveCommand = fabric.moveToPoint(400, 2400);
  if (moveCommand->check(self)) {
    moveCommands.push_back(moveCommand);
  }

  for (const auto& projectile : World::model().getProjectiles()) {
    const auto command = fabric.avoidProjectile(projectile);
    if (command->check(self)) {
      moveCommands.push_back(command);
    }
  }

  CommandStrategy::update(self, move);
}