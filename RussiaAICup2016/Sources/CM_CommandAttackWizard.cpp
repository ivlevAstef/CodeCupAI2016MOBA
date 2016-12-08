#include "CM_CommandAttackWizard.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_AttackPriorities.h"
#include "A_Attack.h"
#include "A_WinPredictor.h"

using namespace AICup;

CommandAttackWizard::CommandAttackWizard(const model::Wizard& wizard) : wizard(wizard) {
}

bool CommandAttackWizard::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto wizardPos = EX::pos(wizard);
  const auto delta = wizardPos - selfPos;

  /// маг далеко
  if (delta.length() > self.getCastRange() + wizard.getRadius()) {
    return false;
  }

  /// Если еще много времени до кд, то не стоит атаковать
  if (self.minStaffOrMissileCooldown() > Algorithm::timeToTurnForAttack(wizard, self) + 1) {
    return false;
  }

  /// если на пути другое дерево, то не стреляем
  if (Algorithm::checkIntersectedTree(selfPos, wizardPos, Game::model().getMagicMissileRadius())) {
    return false;
  }

  return true;
}


void CommandAttackWizard::execute(const Wizard& self, Result& result) {
  result.unit = &wizard;

  if (Algorithm::isMelee(self, wizard) && !self.isCooldown(model::ACTION_STAFF)) {
    result.action = model::ACTION_STAFF;
    result.priority = AttackPriorities::attackWizard(self, wizard, nullptr);
  } else {
    result.action = model::ACTION_MAGIC_MISSILE;

    const auto selfPos = EX::pos(self);
    const auto wizardPos = EX::pos(wizard);
    const auto delta = wizardPos - selfPos;

    /// подвинуть мага на один тик вперед сложно, но зато можно пулю слегка отодвинуть назад, и убедиться что она попадет
    const auto bulletPos = selfPos - delta.normal() * EX::maxSpeed(wizard);
    Bullet bullet = Bullet(0,
      delta.normal() * Game::model().getMagicMissileSpeed(),
      Game::model().getMagicMissileRadius(),
      bulletPos, bulletPos, self.getCastRange(), model::PROJECTILE_MAGIC_MISSILE, self.getFaction());

    result.priority = AttackPriorities::attackWizard(self, wizard, &bullet);
  }

  result.priority *= self.getRole().getWizardPriority();
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