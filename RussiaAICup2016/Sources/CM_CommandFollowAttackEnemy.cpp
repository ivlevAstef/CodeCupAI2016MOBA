#include "CM_CommandFollowAttackEnemy.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"

using namespace AICup;

CommandFollowAttack::CommandFollowAttack(Algorithm::PathFinder& finder, const model::Wizard& wizard): MoveCommand(finder), wizard(wizard) {
}

bool CommandFollowAttack::check(const Wizard& self) {
  return wizard.getLife() < 2 * self.damage(model::ACTION_MAGIC_MISSILE);
}


void CommandFollowAttack::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = wizardPos - selfPos;

  result.moveDirection = delta;
  result.turnStyle = TurnStyle::TURN;
  result.turnPriority = TurnPriority::follow;
}

double CommandFollowAttack::priority(const Wizard& self) {
  return self.getRole().getWizardPriority() * MovePriorities::attackFollow(self, wizard);
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackWizard::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::PRE == visualizator.getStyle()) {
    visualizator.line(self.getX(), self.getY(), wizard.getX(), wizard.getY(), 0xaaffaa);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(wizard.getX(), wizard.getY(), wizard.getRadius() * 0.75, 0xaaffaa);
  }
}
#endif // ENABLE_VISUALIZATOR