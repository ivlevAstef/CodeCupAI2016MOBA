#include "S_TestMoveStrategy.h"
#include "E_World.h"

#include "R_RoundOne.h"

using namespace AICup;

TestMoveStrategy::TestMoveStrategy(const CommandFabric& fabric):
  CommandStrategy(fabric, std::make_shared<RoundOneRole>(), std::make_shared<RoundOneSkillBuild>()) {
  currentMoveCommandIndex = 0;

  allMoveCommands.push_back(fabric.moveToPoint(1200, 1200, 10));
  allMoveCommands.push_back(fabric.moveToLine(model::LANE_BOTTOM));
  allMoveCommands.push_back(fabric.moveToPoint(200, 3800, 10));
  allMoveCommands.push_back(fabric.moveToLine(model::LANE_TOP));
  allMoveCommands.push_back(fabric.moveToLine(model::LANE_MIDDLE));
  allMoveCommands.push_back(fabric.moveToPoint(800, 3200, 10));
  allMoveCommands.push_back(fabric.moveToLine(model::LANE_BOTTOM));
}

void TestMoveStrategy::update(const model::Wizard& model, model::Move& move) {
  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);

  auto& command = allMoveCommands[currentMoveCommandIndex];

  while (!command->check(self)) {
    currentMoveCommandIndex = (currentMoveCommandIndex+1)% allMoveCommands.size();
    allMoveCommands.push_back(allMoveCommands[currentMoveCommandIndex]);
  }

  CommandStrategy::update(self, move);
}