#include "CM_CommandPushOffMinion.h"
#include "E_Game.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"
#include "C_Math.h"

using namespace AICup;

CommandPushOffMinion::CommandPushOffMinion(const model::Minion& minion) : minion(minion) {
  distance = 0;
}

bool CommandPushOffMinion::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(minion);
  const auto delta = selfPos - unitPos;

  distance = minion.getRadius() + self.getRadius() * 2;

  return delta.length() < distance;
}

void CommandPushOffMinion::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(minion);
  const auto delta = selfPos - unitPos;

  position = unitPos + delta.normal() * distance;


  result.set(position, self);
  result.turnStyle = TurnStyle::BACK_TURN;
  result.turnPriority = TurnPriority::avoidMinion;
}

double CommandPushOffMinion::priority(const Wizard& self) {
  return MovePriorities::pushOffMinion(self, minion);
}

#ifdef ENABLE_VISUALIZATOR
void CommandPushOffMinion::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, position.x, position.y, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR