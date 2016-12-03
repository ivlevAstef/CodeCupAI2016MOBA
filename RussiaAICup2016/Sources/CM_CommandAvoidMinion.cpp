#include "CM_CommandAvoidMinion.h"
#include "CM_CommandMoveToPoint.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"
#include "C_Math.h"

using namespace AICup;

CommandAvoidMinion::CommandAvoidMinion(const model::Minion& minion): minion(minion) {
  distance = 0;
}

bool CommandAvoidMinion::check(const Wizard& self) {
  const auto mc = Game::model();

  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(minion);
  const auto delta = selfPos - unitPos;

  if (model::MINION_ORC_WOODCUTTER == minion.getType()) {
    distance = MAX(mc.getStaffRange(), mc.getOrcWoodcutterAttackRange()) + self.getRadius();
  } else {
    distance = mc.getFetishBlowdartAttackRange() + self.getRadius() + mc.getDartRadius();
  }

  if (delta.length() > distance) {
    return false;
  }

  /// если на пути дерево, то тоже бояться не стоит
  if (Algorithm::checkIntersectedTree(selfPos, unitPos, minion.getRadius())) {
    return false;
  }

  return true;
}

void CommandAvoidMinion::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(minion);
  const auto delta = selfPos - unitPos;

  position = unitPos + delta.normal() * distance;


  result.set(position, self);
  result.turnStyle = TurnStyle::BACK_TURN;
  result.turnPriority = TurnPriority::avoidMinion;
}

double CommandAvoidMinion::priority(const Wizard& self) {
  return MovePriorities::avoidMinion(self, minion) * self.getRole().getAudacityMinion();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidMinion::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, position.x, position.y, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR