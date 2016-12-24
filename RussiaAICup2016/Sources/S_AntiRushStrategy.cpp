#include "S_AntiRushStrategy.h"

#include "R_AntiRush.h"
#include "R_StandardInfo.h"
#include "E_World.h"
#include "E_Points.h"
#include "E_InfluenceMap.h"
#include "C_Math.h"

using namespace AICup;

AntiRushStrategy::AntiRushStrategy(const CommandFabric& fabric) :
  /// скилы беруться из стандартной
  BaseStrategyComponents(fabric, nullptr, nullptr) {
}

void AntiRushStrategy::update(const model::Wizard& model, model::Move& move) {
  if (nullptr == skillBuild) {
    skillBuild = StandardInfo::skillBuild(model);
  }

  if (nullptr == role) {
    role = std::make_shared<AntiRushRole>(*StandardInfo::role(model).get());
  }

  CommandStrategy::clear();
  const auto& self = CommandStrategy::preUpdate(model, move);

  ///Атака на мид, из 2 точек (с мида 3, и с нижней "руны" двое)
  addAroundEnemiesOrMoveMelee(self);

  addAttacks(self);
  addCasts(self);


  addAttackFollow(self);

  const auto tacticRole = StandardInfo::tacticsRole(self);

  if (tacticRole == TacticsRole::SupportHaste || tacticRole == TacticsRole::AttackStan) {
    auto distance = Math::distanceToLine(EX::pos(model), Points::point(Points::ACADEMY_BASE), Points::point(Points::RENEGADES_BASE));
    distance = INTERVAL(100, distance - 25, 750);

    const auto pos = InfluenceMap::instance().getForeFront(model::LANE_MIDDLE, -distance);
    addMoveToPoint(self, pos);
  } else {
    addMoveTo(self, model::LANE_MIDDLE);
  }

  CommandStrategy::update(self, move);
}


void AntiRushStrategy::addMoveToPoint(const Wizard& self, Position pos) {
  MoveCommand::Result cache;

  /// эмуляция машины состояний... кривая до невозможности

  double moveToBonusPriority = -1000;
  const auto moveToBonus = fabric.moveToBonus();
  if (moveToBonus->check(self)) {
    moveToBonus->execute(self, cache);
    moveToBonusPriority = cache.priority;
  }


  double moveToPointPriority = -1000;
  const auto moveToPoint = fabric.moveToPoint(pos.x, pos.y, 200);
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