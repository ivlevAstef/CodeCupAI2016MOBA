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
  const double minRange = Game::model().getFireballExplosionMaxDamageRange();
  const double maxRange = Game::model().getFireballExplosionMinDamageRange();

  double priority = 0;

  for (const auto& enemy : World::instance().around(unit, unit.getFaction(), maxRange+100)) {
    double distance = enemy->getDistanceTo(unit);
    distance -= unit.getRadius(); /// так как до края юнита а не до центра

    bool isDynamic = EX::isWizard(*enemy) || EX::isMinion(*enemy);

    /// если объекты движимы, то считаем, что всеже мы их не задеваем на более близкой дистанции
    if (distance > (isDynamic ? (minRange*0.3 + maxRange*0.7) : maxRange)) {
      continue;
    }

    double priorityMult = (distance - minRange) / (maxRange - minRange);
    priorityMult = 1 - MAX(0, priorityMult);

    if (EX::isWizard(*enemy)) {
      priority += priorityMult * 28;
    } else if (EX::isMinion(*enemy)) {
      priority += priorityMult * (enemy->getLife() / 10);
    } else if (EX::isBuilding(*enemy)) {
      if (!Algorithm::isImmortal(EX::asBuilding(*enemy))) {
        priority += priorityMult * 20;
      }
    }
  }

  return priority;
}

bool CommandAttackFireball::check(const Wizard& self) {
  target = nullptr;
  /// скил недоступен
  if (!self.availableAction(model::ACTION_FIREBALL)) {
    return false;
  }

  if (self.cooldown(model::ACTION_FIREBALL) > 0) {
    return false;
  }

  if (self.getMana() < Game::model().getFireballManacost()) {
    return false;
  }


  const auto selfPos = EX::pos(self);

  double maxPriority = 0;
  /// находим кого поджечь
  for (const auto& enemy : World::instance().aroundEnemies(self, self.getCastRange() + 35+50/*чтобы можно было задевать и не по центру врагов*/)) {
    const auto& unit = *enemy;

    const auto unitPos = EX::pos(unit);
    const auto delta = selfPos - unitPos;

    /// если слишком близко то плохо
    if (delta.length() < self.getRadius() + Game::model().getFireballExplosionMinDamageRange() + unit.getRadius()) {
      continue;
    }

    /// если на пути дерево, то не стреляем
    if (Algorithm::checkIntersectedTree(selfPos, unitPos, Game::model().getFireballRadius())) {
      continue;
    }

    const auto priority = calcPriority(unit);
    if (priority > maxPriority) {
      target = &unit;
      maxPriority = priority;
    }
  }

  /// если уж совсем некого бить, то не бьем
  if (maxPriority < 25 / (self.getMana()/Game::model().getFireballManacost())) {
    return false;
  }

  return nullptr != target;
}


void CommandAttackFireball::execute(const Wizard& self, Result& result) {
  assert(nullptr != target);
  result.unit = target;
  result.action = model::ACTION_FIREBALL;
  result.priority = AttackPriorities::attackFireball(self) * self.getRole().getAttackSkillPriority();
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