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

  auto moveCommand = fabric.moveToPoint(200, 800);
  switch ((int)self.getId()) {
    case 1:
    case 2:
    case 6:
    case 7:
      moveCommand = fabric.moveToPoint(200, 800);
      break;
    case 3:
    case 8:
      moveCommand = fabric.moveToPoint(1600, 2400);
      break;
    case 4:
    case 5:
    case 9:
    case 10:
      moveCommand = fabric.moveToPoint(800, 4000-200);
      break;
  }

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