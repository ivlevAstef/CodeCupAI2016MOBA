#include "CM_CommandAttackFireball.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_AttackPriorities.h"
#include "A_Attack.h"

using namespace AICup;

CommandAttackFireball::CommandAttackFireball() {
}

double calcPriority(const model::LivingUnit& unit) {
  double priority = 0;

  for (const auto& enemy : World::instance().around(unit, unit.getFaction(), Game::model().getFireballExplosionMinDamageRange())) {
    if (EX::isWizard(*enemy)) {
      priority += 20;
    } else if (EX::isMinion(*enemy)) {
      priority += enemy->getLife() / 10;
    } else if (EX::isBuilding(*enemy)) {
      priority += 12;
    }
  }
  return priority;
}

bool CommandAttackFireball::check(const Wizard& self) {
  target = nullptr;
  /// ���� ����������
  if (!self.availableAction(model::ACTION_FIREBALL)) {
    return false;
  }

  /// ����� � ��������
  if (self.cooldown(model::ACTION_FIREBALL) > 0) {
    return false;
  }

  // ��� ����
  if (self.getMana() < Game::model().getFireballManacost()) {
    return false;
  }

  const auto selfPos = EX::pos(self);

  double maxPriority = 0;
  /// ������� ���� �������
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getCastRange())) {
    const auto& unit = *enemy;

    const auto unitPos = EX::pos(unit);
    const auto delta = selfPos - unitPos;

    /// ���� ������� ������ �� �����
    if (delta.length() < self.getRadius() + Game::model().getFireballExplosionMinDamageRange() + unit.getRadius()) {
      continue;
    }

    /// ���� �� ���� ������, �� �� ��������
    if (Algorithm::checkIntersectedTree(selfPos, unitPos, Game::model().getFireballRadius())) {
      continue;
    }

    if (EX::isWizard(unit)) {
      const auto& wizard = EX::asWizard(unit);

      const auto fireballSpeed = delta.normal() * Game::model().getFireballSpeed();
      const auto fireballRadius = Game::model().getFireballRadius() + (Game::model().getFireballExplosionMaxDamageRange() + Game::model().getFireballExplosionMinDamageRange()) * 0.5;
      /// ���� ��� ����� ��������� �� �������, �� �� ����� � ���� ��������
      if (Algorithm::canSideForwardEscape(selfPos, self.getCastRange(), wizard, fireballSpeed, fireballRadius)) {
        continue;
      }

      /// ������ ������� ���������
      if (Algorithm::canSideBackwardEscape(selfPos, self.getCastRange(), wizard, fireballSpeed, fireballRadius)) {
        continue;
      }
    }

    const auto priority = calcPriority(unit);
    if (priority > maxPriority) {
      target = &unit;
      maxPriority = priority;
    }
  }

  /// ���� �� ������ ������ ����, �� �� ����
  if (maxPriority < 20) {
    return false;
  }

  return nullptr != target;
}


void CommandAttackFireball::execute(const Wizard&, Result& result) {
  assert(nullptr != target);
  result.unit = target;
  result.action = model::ACTION_FIREBALL;
}

double CommandAttackFireball::priority(const Wizard& self) {
  return AttackPriorities::attackFireball(self) * self.getRole().getAttackSkillPriority();
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackFireball::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  assert(nullptr != target);
  if (Visualizator::PRE == visualizator.getStyle()) {
    visualizator.line(self.getX(), self.getY(), target->getX(), target->getY(), 0xaa00ff);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(target->getX(), target->getY(), target->getRadius() * 0.75, 0xaa00ff);
  }
}
#endif // ENABLE_VISUALIZATOR