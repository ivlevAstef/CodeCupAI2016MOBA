#include "CM_CommandMoveMeleeAttack.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"
#include "A_WinPredictor.h"

using namespace AICup;

CommandMoveMeleeAttack::CommandMoveMeleeAttack(const model::LivingUnit& unit) : unit(unit) {
  changeOfWin = 0;
}

bool CommandMoveMeleeAttack::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(unit);
  const auto delta = unitPos - selfPos;
  if (delta.length() > self.getVisionRange()) {
    return false;
  }

  const auto distanceStaffCD = self.cooldown(model::ACTION_STAFF) * self.maxBackwardSpeed();
  if (delta.length() + self.maxSpeed() < Game::model().getStaffRange() + unit.getRadius() + distanceStaffCD) {
    return false;
  }


  changeOfWin = Algorithm::changeOfWinning(self, self.getRole().getFriendWizardConfidence(), 1, unitPos.x, unitPos.y);
  if (changeOfWin < self.getRole().getAttackMeleeWinThreshold()) {
    return false;
  }

  if (EX::isBuilding(unit)) {
    if (Algorithm::isImmortal(EX::asBuilding(unit))) {
      return false;
    }
  }

  return true;
}


void CommandMoveMeleeAttack::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);
  const auto unitPos = EX::pos(unit);
  const auto delta = unitPos - selfPos;

  if (EX::isWizard(unit)) {
    result.priority = self.getRole().getAttackWizardMeleePriority() * MovePriorities::moveMeleeAttack(self, EX::asWizard(unit));
  } else if (EX::isMinion(unit)) {
    result.priority = self.getRole().getAttackMinionMeleePriority() * MovePriorities::moveMeleeAttack(self, EX::asMinion(unit));
  } else if (EX::isBuilding(unit)) {
    result.priority = self.getRole().getAttackBuildMeleePriority() * MovePriorities::moveMeleeAttack(self, EX::asBuilding(unit));
  }

  result.priority *= changeOfWin;



  result.set(unitPos, self);
  result.turnDirection = Vector();

  result.turnPriority = TurnPriority::follow;
}


#ifdef ENABLE_VISUALIZATOR
void CommandMoveMeleeAttack::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::PRE == visualizator.getStyle()) {
    visualizator.line(self.getX(), self.getY(), unit.getX(), unit.getY(), 0xaaffaa);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(unit.getX(), unit.getY(), unit.getRadius() * 0.75, 0xaaffaa);
  }
}
#endif // ENABLE_VISUALIZATOR