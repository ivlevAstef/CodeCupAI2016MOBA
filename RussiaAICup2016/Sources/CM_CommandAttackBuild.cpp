#include "CM_CommandAttackBuild.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_AttackPriorities.h"
#include "A_Attack.h"

using namespace AICup;

CommandAttackBuild::CommandAttackBuild(const model::Building& build) : build(build) {
}

bool CommandAttackBuild::check(const Wizard& self) {
  const auto selfPos = EX::pos(self);
  const auto buildPos = EX::pos(build);
  const auto delta = selfPos - buildPos;

  double timeForAttack = Algorithm::timeToTurnForAttack(build, self);

  /// здание далеко
  if (delta.length() > self.getCastRange() + build.getRadius()) {
    return false;
  }

  /// Если еще много времени до кд, то не стоит атаковать
  if (self.minStaffOrMissileCooldown() > timeForAttack + 1) {
    return false;
  }

  /// если на пути есть дерево, то не стреляем
  if (Algorithm::checkIntersectedTree(selfPos, buildPos, Game::model().getMagicMissileRadius())) {
    return false;
  }

  /// во всех остальных случаях возможна или атака, или поворот к зданию для атаки
  return true;
}


void CommandAttackBuild::execute(const Wizard& self, Result& result) {
  const double distance = self.getDistanceTo(build);

  result.unit = &build;
  result.priority = self.getRole().getBuildPriority() * AttackPriorities::attackBuild(self, build);

  if (Algorithm::isMelee(self, build) && !self.isCooldown(model::ACTION_STAFF)) {
    result.action = model::ACTION_STAFF;
  } else {
    result.action = model::ACTION_MAGIC_MISSILE;
  }
}

#ifdef ENABLE_VISUALIZATOR
void CommandAttackBuild::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::PRE == visualizator.getStyle()) {
    visualizator.line(self.getX(), self.getY(), build.getX(), build.getY(), 0xff00aa);
  }

  if (Visualizator::POST == visualizator.getStyle()) {
    visualizator.fillCircle(build.getX(), build.getY(), build.getRadius() * 0.75, 0xff00aa);
  }
}
#endif // ENABLE_VISUALIZATOR