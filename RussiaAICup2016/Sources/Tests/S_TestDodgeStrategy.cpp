#include "S_TestDodgeStrategy.h"
#include "E_World.h"

#include "R_RoundTwo.h"

using namespace AICup;

TestDodgeStrategy::TestDodgeStrategy(const CommandFabric& fabric) :
  CommandStrategy(fabric, std::make_shared<RoundTwoRole>(), std::make_shared<RoundTwoSkillBuild>()) {
}

void TestDodgeStrategy::update(const model::Wizard& model, model::Move& move) {
  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);

  /*const auto moveCommand = fabric.moveToPoint(1200, 1200);
  if (moveCommand->check(self)) {
    moveCommands.push_back(moveCommand);
  }*/

  auto moveCommand = fabric.moveToPoint(200, 800, 10);
  switch ((int)self.getId()) {
    case 1:
    case 2:
    case 6:
    case 7:
      moveCommand = fabric.moveToPoint(200, 2600, 10);
      break;
    case 3:
    case 8:
      moveCommand = fabric.moveToPoint(1600, 2400, 10);
      break;
    case 4:
    case 5:
    case 9:
    case 10:
      moveCommand = fabric.moveToPoint(1400, 4000-200, 10);
      break;
  }

  if (moveCommand->check(self)) {
    moveCommands.push_back(moveCommand);
  }

  CommandStrategy::update(self, move);
}