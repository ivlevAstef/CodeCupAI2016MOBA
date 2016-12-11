#include "S_StandardStrategy.h"

#include "E_World.h"

#include "R_StandardInfo.h"

using namespace AICup;

StandardStrategy::StandardStrategy(const CommandFabric& fabric, const Algorithm::PathFinder& pathFinder) :
  BaseStrategyComponents(fabric, pathFinder, nullptr, nullptr) {
  defaultLane = model::_LANE_UNKNOWN_;
}




void StandardStrategy::update(const model::Wizard& model, model::Move& move) {
  if (nullptr == role) {
    role = StandardInfo::role(model);
  }
  if (nullptr == skillBuild) {
    skillBuild = StandardInfo::skillBuild(model);
  }

  if (model::_LANE_UNKNOWN_ == defaultLane) {
    setupDefaultLane(model);
  }

  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);

  if (World::model().getTickIndex() >= 500) {
    changeLane(self, 250);
  } else {
    currentLane = defaultLane;
  }

  ///////////////////////////////////

  addAroundEnemiesOrMoveMelee(self);
  addAttackFollow(self);


  ///////////////////////////////////

  addMoveTo(self, currentLane);

  const auto getExpirienceCommand = fabric.moveGetExpirience();
  if (getExpirienceCommand->check(self)) {
    moveCommands.push_back(getExpirienceCommand);
  }

  ///////////////////////////////////////

  addAttacks(self);
  addCasts(self);

  CommandStrategy::update(self, move);
}

void StandardStrategy::setupDefaultLane(const model::Wizard& self) {
  switch (StandardInfo::tacticsRole(self)) {
    case TacticsRole::TankStan:
    case TacticsRole::TankAttack:
      defaultLane = model::LANE_MIDDLE;
      break;
    case TacticsRole::SupportStan:
    case TacticsRole::AttackAOE:
      defaultLane = model::LANE_TOP;
      break;
    case TacticsRole::SupportHaste:
    case TacticsRole::AttackStan:
      defaultLane = model::LANE_BOTTOM;
      break;
  }
}