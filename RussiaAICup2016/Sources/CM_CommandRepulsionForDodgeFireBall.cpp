#include "CM_CommandRepulsionForDodgeFireBall.h"
#include "E_Game.h"
#include "E_World.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"

using namespace AICup;

CommandRepulsionForDodgeFireBall::CommandRepulsionForDodgeFireBall() {
}

bool CommandRepulsionForDodgeFireBall::check(const Wizard& self) {
  bool foundFireballEnemy = false;
  for (const auto& wizard : World::instance().wizards()) {
    if (wizard.getFaction() == Game::enemyFaction()
      && EX::availableSkill(wizard, model::ACTION_FIREBALL)
      && wizard.getDistanceTo(self) < 700) {
      foundFireballEnemy = true;
      break;
    }
  }

  /// если рядом нет fire магов то расходиться не нужно
  if (!foundFireballEnemy) {
    return false;
  }

  /// если есть маг от которого надо отталкиваться то значит это нужно делать
  for (const auto& wizard : World::instance().wizards()) {
    if (wizard.getFaction() == self.getFaction()
      && wizard.getDistanceTo(self) < Game::model().getFireballExplosionMinDamageRange() + self.getRadius()) {
      return true;
    }
  }

  return false;
}

void CommandRepulsionForDodgeFireBall::execute(const Wizard& self, Result& result) {
  const auto selfPos = EX::pos(self);

  const double length = Game::model().getFireballExplosionMinDamageRange() + self.getRadius();

  /// находим всех магов, к которым мы близко стоим и складываем направления чтобы двигаться от них
  Vector dodgeVector;
  double priority = 0;
  for (const auto& wizard : World::instance().wizards()) {
    if (wizard.getFaction() == self.getFaction()
      && wizard.getDistanceTo(self) < length) {
      Vector delta = selfPos - EX::pos(wizard);
      dodgeVector += delta.normal() * (length - delta.length());
      priority += delta.length();
    }
  }

  result.set(selfPos + dodgeVector, self);

  result.turnPriority = TurnPriority::avoidMinion;
  result.priority = priority * self.getRole().getRepulsionDodgeFireballPriority();
  result.force = false;
}

#ifdef ENABLE_VISUALIZATOR
void CommandRepulsionForDodgeFireBall::visualization(const model::Wizard&, const Visualizator&) const {
}
#endif // ENABLE_VISUALIZATOR