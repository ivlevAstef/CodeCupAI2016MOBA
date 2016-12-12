#include "S_AntiRushStrategy.h"

#include "R_AntiRush.h"
#include "R_StandardInfo.h"
#include "E_World.h"
#include "E_Points.h"

using namespace AICup;

AntiRushStrategy::AntiRushStrategy(const CommandFabric& fabric) :
  /// скилы беруться из стандартной
  BaseStrategyComponents(fabric, nullptr, nullptr) {
}

void AntiRushStrategy::update(const model::Wizard& model, model::Move& move) {
  if (nullptr == skillBuild) {
    skillBuild = StandardInfo::skillBuild(model);
  }

  bool boolHasCenter = false;
  for (const auto& wizard : World::model().getWizards()) {
    if (wizard.getFaction() == model.getFaction()) {
      boolHasCenter |= (4 == World::instance().aroundAuraWizards(model).size());
    }
  }

  if (boolHasCenter) {
    role = std::make_shared<AntiRushRole>();
  } else {
    role = StandardInfo::role(model);
  }

  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);


  ///Атака на мид, из 2 точек (с мида 3, и с нижней "руны" двое)
  addAroundEnemiesOrMoveMelee(self);

  addAttacks(self);
  addCasts(self);


  addAttackFollow(self);

  /*if (model::LANE_BOTTOM == World::instance().positionToLine(model.getX(), model.getY())) {
    addMoveToBottom(self);
  } else {*/
    addMoveTo(self, model::LANE_MIDDLE);
  //}



  CommandStrategy::update(self, move);
}


void AntiRushStrategy::addMoveToBottom(const Wizard& self) {
  MoveCommand::Result cache;

  /// эмуляция машины состояний... кривая до невозможности

  double moveToBonusPriority = -1000;
  const auto moveToBonus = fabric.moveToBonus();
  if (moveToBonus->check(self)) {
    moveToBonus->execute(self, cache);
    moveToBonusPriority = cache.priority;
  }


  double moveToPointPriority = -1000;
  const auto pos = Points::point(Points::BONUS_BOTTOM) + Vector(-500, -500);
  const auto moveToPoint = fabric.moveToPoint(pos.x, pos.y, 2000);
  if (moveToPoint->check(self)) {
    moveToPoint->execute(self, cache);
    moveToPointPriority = cache.priority;
  }

  if (moveToPointPriority < 0 && moveToBonusPriority < 0) {
    return;
  }


  if (moveToPointPriority >= moveToBonusPriority) {
    moveCommands.push_back(moveToPoint);
  } else {
    moveCommands.push_back(moveToBonus);
  }
}