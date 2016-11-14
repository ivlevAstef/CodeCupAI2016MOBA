#include "CM_TestMoveStrategy.h"
#include "E_World.h"

using namespace AICup;

TestMoveStrategy::TestMoveStrategy(const CommandFabric& fabric): CommandStategy(fabric) {
  currentMoveCommandIndex = 0;

  moveCommands.push_back(fabric.moveToPoint(1200, 1200));
  moveCommands.push_back(fabric.moveToLine(model::LANE_BOTTOM));
  moveCommands.push_back(fabric.moveToPoint(200, 3800));
  moveCommands.push_back(fabric.moveToLine(model::LANE_TOP));
  moveCommands.push_back(fabric.moveToLine(model::LANE_MIDDLE));
  moveCommands.push_back(fabric.moveToPoint(800, 3200));
  moveCommands.push_back(fabric.moveToLine(model::LANE_BOTTOM));
}

void TestMoveStrategy::update(const model::Wizard& self, model::Move& move) {
  auto& command = moveCommands[currentMoveCommandIndex];

  while (!command->check(self, move)) {
    currentMoveCommandIndex = (currentMoveCommandIndex+1)% moveCommands.size();
    command = moveCommands[currentMoveCommandIndex];
  }

  command->execute(self, move);
}

#ifdef ENABLE_VISUALIZATOR

void TestMoveStrategy::visualization(const Visualizator& visualizator) const {
  const auto& command = moveCommands[currentMoveCommandIndex];
  command->visualization(visualizator);
}

#endif
