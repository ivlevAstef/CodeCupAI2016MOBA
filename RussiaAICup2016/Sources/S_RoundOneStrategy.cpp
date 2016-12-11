#include "S_RoundOneStrategy.h"

#include "R_RoundOne.h"

using namespace AICup;

RoundOneStrategy::RoundOneStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder) :
  BaseStrategyComponents(fabric, pathFinder, std::make_shared<RoundOneRole>(), std::make_shared<RoundOneSkillBuild>()) {
}

void RoundOneStrategy::update(const model::Wizard& model, model::Move& move) {
  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model,move);

  const auto lane = checkAndChangeLane(self);

  ///////////////////////////////////

  addAroundEnemiesOrMoveMelee(self);
  addAttackFollow(self);


  ///////////////////////////////////

  addMoveTo(self, lane);

  const auto getExpirienceCommand = fabric.moveGetExpirience();
  if (getExpirienceCommand->check(self)) {
    moveCommands.push_back(getExpirienceCommand);
  }

  ///////////////////////////////////////

  addAttacks(self);
  addCasts(self);

  CommandStrategy::update(self, move);
}