#include "CM_CommandAvoidBuild.h"
#include "CM_CommandMoveToPoint.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"

using namespace AICup;

CommandAvoidBuild::CommandAvoidBuild(Algorithm::PathFinder& finder, const model::Building& build) :
  MoveCommand(finder), build(build) {
}

bool CommandAvoidBuild::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto buildPos = EX::pos(build);
  const auto delta = selfPos - buildPos;

  double distance = build.getAttackRange() - build.getRemainingActionCooldownTicks() * self.maxBackwardSpeed() + self.getRadius();
  distance += self.maxBackwardSpeed(); /// небольшой запасец

  if (delta.length() > distance) {
    return false;
  }

  /// ближайшая точка, где безопасно
  const auto pos = buildPos + delta.normal() * distance;
  moveToPointCommand = std::make_shared<CommandMoveToPoint>(pathFinder, pos.x, pos.y, TurnStyle::BACK_TURN);

  return moveToPointCommand->check(self);
}

void CommandAvoidBuild::execute(const Wizard& self, Result& result) {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->execute(self, result);
  result.turnPriority = TurnPriority::avoidBuild;
}

double CommandAvoidBuild::priority(const Wizard& self) {
  return MovePriorities::avoidBuild(self, build) * self.getRole().getAudacityBuild();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidBuild::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != moveToPointCommand.get());
  moveToPointCommand->visualization(self, visualizator);
}
#endif // ENABLE_VISUALIZATOR