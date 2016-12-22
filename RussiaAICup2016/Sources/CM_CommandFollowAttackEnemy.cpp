#include "CM_CommandFollowAttackEnemy.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"
#include "A_WinPredictor.h"

using namespace AICup;

CommandFollowAttack::CommandFollowAttack(const model::Wizard& wizard): wizard(wizard) {
}

bool CommandFollowAttack::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = wizardPos - selfPos;
  if (delta.length() > self.getVisionRange() + 150) {
    return false;
  }

  const auto changeOfWin = Algorithm::changeOfWinning(self, wizardPos.x, wizardPos.y);

  // если у мага мало хп и шанс на победу есть, то стоит преследовать
  return (wizard.getLife() + 1 < self.damage(model::ACTION_MAGIC_MISSILE) && changeOfWin > 0)
  /// если шанс на победу при нападении средний, то тоже преследуем
    || changeOfWin > 0.2;
}


void CommandFollowAttack::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = wizardPos - selfPos;

  result.set(wizardPos, self);
  result.turnDirection = Vector();

  result.turnPriority = TurnPriority::follow;
  result.priority = self.getRole().getWizardPriority() * MovePriorities::attackFollow(self, wizard);
}


#ifdef ENABLE_VISUALIZATOR
void CommandFollowAttack::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::PRE == visualizator.getStyle()) {
    visualizator.line(self.getX(), self.getY(), wizard.getX(), wizard.getY(), 0xaaffaa);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(wizard.getX(), wizard.getY(), wizard.getRadius() * 0.75, 0xaaffaa);
  }
}
#endif // ENABLE_VISUALIZATOR