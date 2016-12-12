#include "S_TestMoveAndAttackStrategy.h"
#include "E_World.h"

#include "R_RoundOne.h"

using namespace AICup;

TestMoveAndAttackStrategy::TestMoveAndAttackStrategy(const CommandFabric& fabric):
  CommandStrategy(fabric, std::make_shared<RoundOneRole>(), std::make_shared<RoundOneSkillBuild>()) {
  currentMoveCommandIndex = 0;

  allMoveCommands.push_back(fabric.moveToPoint(800, 3200, 10));
  allMoveCommands.push_back(fabric.moveToPoint(200, 2400, 10));
  allMoveCommands.push_back(fabric.moveToPoint(1200, 1200, 10));
  allMoveCommands.push_back(fabric.moveToPoint(1600, 2400, 10));
  allMoveCommands.push_back(fabric.moveToPoint(1200, 3800, 10));
  allMoveCommands.push_back(fabric.moveToPoint(2800, 3800, 10));
  allMoveCommands.push_back(fabric.moveToPoint(2800, 2800, 10));
  allMoveCommands.push_back(fabric.moveToPoint(1600, 2400, 10));
}

void TestMoveAndAttackStrategy::update(const model::Wizard& model, model::Move& move) {
  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);

  if (2.5 * 2.5 > self.getSpeedX()*self.getSpeedX() + self.getSpeedY()*self.getSpeedY()) {
    printf("Small Speed: %f %f\n", self.getSpeedX(), self.getSpeedY());
  }

  for (const auto& enemy : World::instance().aroundEnemies(self, self.getVisionRange())) {
    const auto attackCommand = fabric.attack(*enemy);
    if (attackCommand->check(self)) {
      attackCommands.push_back(attackCommand);
      break;
    }
  }

  auto& command = allMoveCommands[currentMoveCommandIndex];

  while (!command->check(self)) {
    currentMoveCommandIndex = (currentMoveCommandIndex + 1) % allMoveCommands.size();
    command = allMoveCommands[currentMoveCommandIndex];
  }

  moveCommands.push_back(command);

  CommandStrategy::update(self, move);
}