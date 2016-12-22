#include "S_RoundTwoStrategy.h"
#include "E_Game.h"
#include "E_World.h"

#include "R_RoundTwo.h"

using namespace AICup;

#ifdef FROM_FILE
RoundTwoRole RoundTwoRole::initializer = RoundTwoRole("balance_current.txt");
#endif // FROM_FILE


RoundTwoStrategy::RoundTwoStrategy(const CommandFabric& fabric) :
  BaseStrategyComponents(fabric, std::make_shared<RoundTwoRole>(), std::make_shared<RoundTwoSkillBuild>()) {
}

static model::LaneType laneWithIndex(model::LaneType lane, int index) {
  if (lane == model::LANE_BOTTOM) {
    return model::LaneType((int(lane) + model::_LANE_COUNT_ - index) % model::_LANE_COUNT_);
  }
  return model::LaneType((int(lane) + index) % model::_LANE_COUNT_);
}

void RoundTwoStrategy::update(const model::Wizard& model, model::Move& move) {
  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);

  if (100 <= World::model().getTickIndex() && World::model().getTickIndex() <= 500) {
    changeLane(self);
  } else {
    changeLane(self, 250);
  }

  ///////////////////////////////////

  addAroundEnemiesOrMoveMelee(self);
  addAttackFollow(self);


  ///////////////////////////////////

  addMoveTo(self, currentLane);
  repulsionDodgeFireball(self);

  const auto getExpirienceCommand = fabric.moveGetExpirience();
  if (getExpirienceCommand->check(self)) {
    moveCommands.push_back(getExpirienceCommand);
  }

  ///////////////////////////////////////

  addAttacks(self);
  addCasts(self);

  CommandStrategy::update(self, move);
}