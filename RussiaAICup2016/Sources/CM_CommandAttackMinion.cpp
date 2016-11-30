#include "CM_CommandAttackMinion.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_AttackPriorities.h"
#include "A_Attack.h"

using namespace AICup;

CommandAttackMinion::CommandAttackMinion(const model::Minion& minion) : minion(minion) {
}

bool CommandAttackMinion::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto minionPos = EX::pos(minion);
  const auto minionSpeed = Vector(minion.getSpeedX(), minion.getSpeedY());
  const auto futureMinionPos = EX::pos(minion) + ((minionSpeed.length() > 0.1) ? (minionSpeed.normal() * minion.getRadius() * 0.75) : Vector());
  const auto delta = selfPos - futureMinionPos;

  /// миньон далеко или будет далеко
  if (delta.length() > self.getCastRange()) {
    return false;
  }

  /// Если еще много времени до кд, то не стоит атаковать
  if (self.minStaffOrMissileCooldown() > Algorithm::timeToTurnForAttack(minion, self)) {
    return false;
  }

  /// если на пути есть дерево, то не стреляем
  if (Algorithm::checkIntersectedTree(selfPos, futureMinionPos, Game::model().getMagicMissileRadius())) {
    return false;
  }

  /// во всех остальных случаях возможна или атака, или поворот к зданию для атаки
  return true;
}


void CommandAttackMinion::execute(const Wizard& self, Result& result) {
  const double distance = self.getDistanceTo(minion);

  result.unit = &minion;

  if (Algorithm::isMelee(self, minion) && !self.isCooldown(model::ACTION_STAFF)) {
    result.action = model::ACTION_STAFF;
  } else {
    result.action = model::ACTION_MAGIC_MISSILE;
  }
}

double CommandAttackMinion::priority(const Wizard& self) {
  return self.getRole().getMinionPriority() * AttackPriorities::attackMinion(self, minion);
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackMinion::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::PRE == visualizator.getStyle()) {
    visualizator.line(self.getX(), self.getY(), minion.getX(), minion.getY(), 0xffaaaa);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(minion.getX(), minion.getY(), minion.getRadius() * 0.75, 0xffaaaa);
  }
}
#endif // ENABLE_VISUALIZATOR