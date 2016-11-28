#include "CM_CommandAttackFrostbolt.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_AttackPriorities.h"
#include "A_Attack.h"

using namespace AICup;

CommandAttackFrostbolt::CommandAttackFrostbolt() {
}

bool CommandAttackFrostbolt::check(const Wizard& self) {
  target = nullptr;
  /// скил недоступен
  if (!self.availableAction(model::ACTION_FROST_BOLT)) {
    return false;
  }

  /// скилл в кулдауне
  if (self.cooldown(model::ACTION_FROST_BOLT) > 0) {
    return false;
  }

  // нет маны
  if (self.getMana() < Game::model().getFrostBoltManacost()) {
    return false;
  }

  const auto selfPos = EX::pos(self);

  double maxPriority = 0;
  /// находим кого заморозить
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getCastRange())) {
    if (EX::isWizard(*enemy)) {
      const auto& wizard = EX::asWizard(*enemy);

      const auto wizardPos = EX::pos(wizard);
      const auto delta = wizardPos - selfPos;

      /// если на пути дерево, то не стреляем
      if (Algorithm::checkIntersectedTree(selfPos, wizardPos, Game::model().getFrostBoltRadius())) {
        continue;
      }

      /// если маг может уклонится от снаряда, то не будем в него стрелять
      const auto frostBoltSpeed = delta.normal() * Game::model().getFrostBoltSpeed();
      if (Algorithm::canForwardEscape(selfPos, self.getCastRange(), wizard, frostBoltSpeed, Game::model().getFrostBoltRadius())) {
        continue;
      }

      /// второй вариант уклонения
      if (Algorithm::canBackwardEscape(selfPos, self.getCastRange(), wizard, frostBoltSpeed, Game::model().getFrostBoltRadius())) {
        continue;
      }

      const auto priority = self.getRole().getWizardPriority() * AttackPriorities::attackWizard(self, wizard);
      if (priority > maxPriority) {
        target = &wizard;
        maxPriority = priority;
      }

    }

    if (EX::isMinion(*enemy) && Algorithm::isMelee(self, *enemy)) {
      const auto& minion = EX::asMinion(*enemy);

      const auto priority = self.getRole().getMinionPriority() * AttackPriorities::attackMinion(self, minion);
      if (priority > maxPriority) {
        target = &minion;
        maxPriority = priority;
      }
    }
  }

  return nullptr != target;
}


void CommandAttackFrostbolt::execute(const Wizard& self, Result& result) {
  assert(nullptr != target);
  result.unit = target;
  result.action = model::ACTION_FROST_BOLT;
}

double CommandAttackFrostbolt::priority(const Wizard& self) {
  return AttackPriorities::attackFrostbolt(self);
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackFrostbolt::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != target);
  if (Visualizator::PRE == visualizator.getStyle()) {
    visualizator.line(self.getX(), self.getY(), target->getX(), target->getY(), 0xaa00ff);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(target->getX(), target->getY(), target->getRadius() * 0.75, 0xaa00ff);
  }
}
#endif // ENABLE_VISUALIZATOR