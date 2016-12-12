#include "S_Mid5RushStrategy.h"

#include "R_Rush.h"

using namespace AICup;

Mid5RushStrategy::Mid5RushStrategy(const CommandFabric& fabric) :
  BaseStrategyComponents(fabric, std::make_shared<RushRole>(), std::make_shared<RushSkillBuild>()) {
}

void Mid5RushStrategy::update(const model::Wizard& model, model::Move& move) {
  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);

  ///////////////////////////////////

  addAroundEnemiesOrMoveMelee(self);
  addAttackFollow(self);

  ///////////////////////////////////

  addMoveTo(self, model::LANE_MIDDLE);

  const auto getExpirienceCommand = fabric.moveGetExpirience();
  if (getExpirienceCommand->check(self)) {
    moveCommands.push_back(getExpirienceCommand);
  }

  ///////////////////////////////////////

  addAttacks(self);
  addCasts(self);

  CommandStrategy::update(self, move);
}