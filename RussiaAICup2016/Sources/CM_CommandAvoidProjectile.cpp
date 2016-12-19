#include "CM_CommandAvoidProjectile.h"
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

  auto lProjectile = projectile;
  double fullRadius = projectile.radius;
  if (lProjectile.type == model::PROJECTILE_FIREBALL) {
    lProjectile.radius = Game::model().getFireballExplosionMaxDamageRange();
    fullRadius = Game::model().getFireballExplosionMinDamageRange();
  }

  const auto selfPos = EX::pos(self);
  const auto projectileEndPos = lProjectile.startPoint + lProjectile.speed.normal() * projectile.range;

  const auto distanceToLine = Math::distanceToSegment(selfPos, lProjectile.startPoint, projectileEndPos);
  const auto distanceMoved = self.getRadius() + fullRadius - distanceToLine;

  /// ���� ������ �� � ���, � � �� ���� � ���� ����� �� ��� ������������
  if (distanceMoved < -self.maxSpeed()) {
    return false;
  }

  int turnSign = 0;
  const auto dodgeVector = Algorithm::dodge(self, moveDirection, lProjectile, turnSign);
  /// ���������� �����������
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
  result.force = true;
}

#ifdef ENABLE_VISUALIZATOR
void CommandAvoidProjectile::visualization(const model::Wizard& self, const Visualizator& visualizator) const {
  if (Visualizator::POST == visualizator.getStyle()) {
    const auto from = EX::pos(self);

    visualizator.line(from.x, from.y, position.x, position.y, 0xff0000);
  }
}
#endif // ENABLE_VISUALIZATOR