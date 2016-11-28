#include "CM_CommandAttackWizard.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_AttackPriorities.h"
#include "A_Attack.h"

using namespace AICup;

CommandAttackWizard::CommandAttackWizard(const model::Wizard& wizard) : wizard(wizard) {
}

bool CommandAttackWizard::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = wizardPos - selfPos;

  /// маг далеко
  if (delta.length() > self.getCastRange()) {
    return false;
  }

  /// Если еще много времени до кд, то не стоит атаковать
  if (self.minStaffOrMissileCooldown() > Algorithm::timeToTurnForAttack(wizard, self)) {
    return false;
  }

  /// если на пути другое дерево, то не стреляем
  if (Algorithm::checkIntersectedTree(selfPos, wizardPos, Game::model().getMagicMissileRadius())) {
    return false;
  }

  /// если враг очень близко, и мы можем бить посохом, то надо ударить
  if (Algorithm::isMelee(self, wizard) && !self.isCooldown(model::ACTION_STAFF)) {
    return true;
  }

  /// если маг может уклонится от снаряда, то не будем в него стрелять
  const auto bulletSpeed = delta.normal() * Game::model().getMagicMissileSpeed();
  if (Algorithm::canForwardEscape(selfPos, self.getCastRange(), wizard, bulletSpeed, Game::model().getMagicMissileRadius())) {
    return false;
  }

  /// второй вариант уклонения, пока никто не реализовывал, поэтому закоменчен
  /*if (Algorithm::canBackwardEscape(selfPos, self.getCastRange(), wizard, Game::model().getMagicMissileSpeed(), Game::model().getMagicMissileRadius())) {
    return false;
  }*/

  /// во всех остальных случаях возможна или атака, или поворот к дереву для атаки
  return true;
}


void CommandAttackWizard::execute(const Wizard& self, Result& result) {
  const double distance = self.getDistanceTo(wizard);

  result.unit = &wizard;

  if (Algorithm::isMelee(self, wizard) && !self.isCooldown(model::ACTION_STAFF)) {
    result.action = model::ACTION_STAFF;
  } else {
    result.action = model::ACTION_MAGIC_MISSILE;
  }
}

double CommandAttackWizard::priority(const Wizard& self) {
  return self.getRole().getWizardPriority() * AttackPriorities::attackWizard(self, wizard);
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackWizard::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::PRE == visualizator.getStyle()) {
    visualizator.line(self.getX(), self.getY(), wizard.getX(), wizard.getY(), 0xff0000);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(wizard.getX(), wizard.getY(), wizard.getRadius() * 0.75, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR