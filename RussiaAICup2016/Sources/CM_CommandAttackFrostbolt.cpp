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

  const auto selfPos = EX::pos(self);

  double maxPriority = 0;
  /// находим кого заморозить
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getCastRange()+35+15)) {
    double timeForAttack = Algorithm::timeToTurnForAttack(*enemy, self);

    if (self.cooldown(model::ACTION_FROST_BOLT) > timeForAttack + 1) {
      continue;
    }

    double manaRestore = Game::model().getWizardBaseManaRegeneration() + self.getLevel() * Game::model().getWizardManaRegenerationGrowthPerLevel();
    if (self.getMana() + manaRestore * timeForAttack < Game::model().getFrostBoltManacost()) {
      continue;
    }

    if (EX::isWizard(*enemy)) {
      const auto& wizard = EX::asWizard(*enemy);

      const auto wizardPos = EX::pos(wizard);
      const auto delta = wizardPos - selfPos;

      /// игнорируем юнитов которые могут увернуться даже если идти боком
      if (delta.length() > EX::radiusForGuaranteedDodgeFrostBolt(wizard, 0)) {
        continue;
      }

      /// если на пути дерево, то не стреляем
      if (Algorithm::checkIntersectedTree(selfPos, wizardPos, Game::model().getFrostBoltRadius())) {
        continue;
      }

      /// подвинуть мага на один тик вперед сложно, но зато можно пулю слегка отодвинуть назад, и убедиться что она попадет
      const auto bulletPos = selfPos - delta.normal() * EX::maxSpeed(wizard);
      Bullet bullet = Bullet(0,
        delta.normal() * Game::model().getFrostBoltSpeed(),
        Game::model().getFrostBoltRadius(),
        bulletPos, bulletPos, self.getCastRange(), model::PROJECTILE_FROST_BOLT, self.getFaction());

      /// если маг может уклонится от снаряда, то приоритет занизится него стрелять
      /*если мы скоро можем умереть то что тратить жизнь в пустую хоть на последок кинуть стан*/
      if (self.getLife() > 24 && Algorithm::canDodge(wizard, Vector(1, 0).rotate(wizard.getAngle()), bullet)) {
        continue;
      }

      const auto priority = self.getRole().getWizardPriority() * AttackPriorities::attackWizard(self, wizard, nullptr);
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
  result.priority = AttackPriorities::attackFrostbolt(self) * self.getRole().getAttackSkillPriority();
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