#include "CM_CommandAvoidProjectile.h"
#include "CM_CommandMoveToPoint.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "CM_MovePriorities.h"
#include "CM_TurnPriority.h"
#include "A_Attack.h"

using namespace AICup;

CommandAvoidProjectile::CommandAvoidProjectile(const Bullet& projectile, const Vector& moveDirection):
  projectile(projectile), moveDirection(moveDirection) {
}

bool CommandAvoidProjectile::check(const Wizard& self) {
  const auto mc = Game::model();
  double projectileRadius = projectile.radius; /// специальный радиус, чтобы убегать от взрыва дабы снизить урон
  if (projectile.type == model::PROJECTILE_FIREBALL) {
    projectileRadius += Game::model().getFireballExplosionMinDamageRange();
  }


  const auto selfPos = EX::pos(self);

  const auto distanceToLine = Math::distanceToSegment(selfPos, projectile.startPoint, projectile.startPoint + projectile.speed.normal() * projectile.range);
  const auto distanceMoved = self.getRadius() + projectileRadius - distanceToLine;

  /// если снаряд не в нас, и я не могу в него войти то что беспокоиться
  if (distanceMoved < -self.maxSpeed()) {
    return false;
  }

  int turnSign = 0;
  const auto dodgeVector = Algorithm::dodge(self, moveDirection, projectile, turnSign);
  /// невозможно отклониться
  if (dodgeVector.length() < 1.0e-5) {
    return false;
  }

  turnDirection = dodgeVector.normal() * turnSign;
  position = selfPos + dodgeVector * self.maxSpeed();

  return true;
}

void CommandAvoidProjectile::execute(const Wizard& self, Result& result) {
  result.set(position, self);
  result.turnDirection = turnDirection;

  result.turnPriority = TurnPriority::avoidProjectile;
  result.priority = MovePriorities::avoidProjectile(self, projectile);
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidProjectile::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, position.x, position.y, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR