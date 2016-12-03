#include "CM_CommandAvoidBuild.h"
#include "CM_CommandMoveToPoint.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"

using namespace AICup;

///http://russianaicup.ru/game/view/86164 отдалься башне почти в начале
///http://russianaicup.ru/game/view/86165 получил от вышки вначале
CommandAvoidBuild::CommandAvoidBuild(const model::Building& build): build(build) {
  distance = 0;
}

bool CommandAvoidBuild::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto buildPos = EX::pos(build);
  const auto delta = selfPos - buildPos;

  distance = build.getAttackRange() - build.getRemainingActionCooldownTicks() * self.maxBackwardSpeed();
  distance += self.maxSpeed() + self.getRadius(); /// небольшой запасец, ибо есть погрешность с позиционированием, да и невсегда все идет идеально когда уходишь от башни

  return delta.length() < distance;
}

void CommandAvoidBuild::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto buildPos = EX::pos(build);
  const auto delta = selfPos - buildPos;

  position = buildPos + delta.normal() * distance;

  result.set(position, self);
  result.turnStyle = TurnStyle::TURN;
  result.turnPriority = TurnPriority::avoidBuild;
}

double CommandAvoidBuild::priority(const Wizard& self) {
  return MovePriorities::avoidBuild(self, build) * self.getRole().getAudacityBuild();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidBuild::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, position.x, position.y, 0xff00ff);
  }
}
#endif // ENABLE_VISUALIZATOR