#include "CM_CommandAvoidMinion.h"
#include "CM_CommandMoveToPoint.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"

using namespace AICup;

CommandAvoidMinion::CommandAvoidMinion(Algorithm::PathFinder& finder, const model::Minion& minion) :
  MoveCommand(finder), minion(minion) {

}

bool CommandAvoidMinion::check(const Wizard& self) {
  const auto mc = Game::model();

  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(minion);
  const auto delta = selfPos - unitPos;

  double distance = 0;
  if (model::MINION_ORC_WOODCUTTER == minion.getType()) {
    distance = mc.getOrcWoodcutterAttackRange() + self.getRadius() + 50/*на всякий случай*/;
  } else {
    distance = mc.getFetishBlowdartAttackRange() + self.getRadius() + mc.getDartRadius();
  }

  if (delta.length() > distance) {
    return false;
  }

  /// если на пути дерево, то тоже бояться не стоит
  if (Algorithm::checkIntersectedTree(selfPos, unitPos, Game::model().getDartRadius())) {
    return false;
  }

  /// точка где безопасно
  const auto pos = unitPos + delta.normal() * distance;

  moveToPointCommand = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::BACK_TURN);

  return moveToPointCommand->check(self);
}

void CommandAvoidMinion::execute(const Wizard& self, Result& result) {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->execute(self, result);
  result.turnPriority = TurnPriority::avoidMinion;
}

double CommandAvoidMinion::priority(const Wizard& self) {
  return MovePriorities::avoidMinion(self, minion) * self.getRole().getMinionPriority();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidMinion::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->visualization(self, visualizator);
}
#endif // ENABLE_VISUALIZATOR