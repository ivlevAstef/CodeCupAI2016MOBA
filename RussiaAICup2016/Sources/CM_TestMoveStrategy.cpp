#include "CM_TestMoveStrategy.h"

using namespace AICup;

TestMoveStrategy::TestMoveStrategy(const CommandFabric& fabric): CommandStategy(fabric) {
  currentMoveCommandIndex = 0;
  moveCommands.push_back(fabric.moveToPoint(1200, 1200));
  moveCommands.push_back(fabric.moveToLine(model::LINE_BOTTOM));
  moveCommands.push_back(fabric.moveToPoint(200, 3800));
  moveCommands.push_back(fabric.moveToLine(model::LINE_TOP));
  moveCommands.push_back(fabric.moveToLine(model::LINE_MIDDLE));
  moveCommands.push_back(fabric.moveToPoint(800, 3200));
  moveCommands.push_back(fabric.moveToLine(model::LINE_BOTTOM));
}

void TestMoveStrategy::update(const model::Wizard& self, model::Move& move) {
  auto& command = moveCommands[currentMoveCommandIndex];

  while (!command->check(self, move)) {
    currentMoveCommandIndex = (currentMoveCommandIndex+1)% moveCommands.size();
    command = moveCommands[currentMoveCommandIndex];
  }

  command->execute(self, move);
}

void TestMoveStrategy::visualization(const Visualizator& visualizator) const {
  const auto& command = moveCommands[currentMoveCommandIndex];
  command->visualization(visualizator);
}
