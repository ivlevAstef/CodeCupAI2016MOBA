#include "A_Attack.h"
#include "E_World.h"
#include "E_Game.h"
#include "C_Math.h"
#include "C_Extensions.h"
#include "A_Move.h"

using namespace AICup;

bool Algorithm::checkIntersectedTree(const Position& p1, const Position& p2, const double radius) {
  for (const auto& tree : World::instance().trees()) {
    if (Math::distanceToSegment(EX::pos(tree), p1, p2) < tree.getRadius() + radius) {
      return true;
    }
  }

  return false;
}

bool Algorithm::checkIntersectedTree(const Position& p1, const Position& p2, const double radius, const long long ignoreId) {
  for (const auto& tree : World::instance().trees()) {
    if (tree.getId() == ignoreId) {
      continue;
    }

    if (Math::distanceToSegment(EX::pos(tree), p1, p2) < tree.getRadius() + radius) {
      return true;
    }
  }

  return false;
}


bool Algorithm::isMelee(const model::Wizard& wizard, const model::LivingUnit& unit) {
  return wizard.getDistanceTo(unit) < Game::model().getStaffRange() + unit.getRadius();
}

double Algorithm::timeToTurnForAttack(const model::Unit& attacked, const model::Wizard& attacking) {
  const auto selfPos = Position(attacking.getX(), attacking.getY());
  const auto enemyPos = Position(attacked.getX(), attacked.getY());

  const auto dir = enemyPos - selfPos;
  double angleDeviation = Math::angleDiff(dir.angle(), attacking.getAngle());
  angleDeviation = ABS(angleDeviation);

  const double needTurnAngle = MAX(0, angleDeviation - Game::instance().model().getStaffSector() * 0.5);
  return needTurnAngle / EX::turnSpeed(attacking);
}

double Algorithm::timeToTurn(const model::Wizard& wizard, const double angle) {
  double angleDeviation = Math::angleDiff(angle, wizard.getAngle());
  angleDeviation = ABS(angleDeviation);

  const double needTurnAngle = MAX(0, angleDeviation);
  return needTurnAngle / EX::turnSpeed(wizard);
}

bool canEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius, const double sign) {
  auto preyPos = EX::pos(prey);
  auto preyAngle = prey.getAngle();
  auto preyTurnSpeed = EX::turnSpeed(prey);

  auto preyEndAngleVec = bulletSpeed.perpendicular().normal();
  /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикул€ров
  if (preyEndAngleVec.dot(Vector(1, 0).rotate(preyAngle)) < 0) {
    preyEndAngleVec *= -1;
  }
  preyEndAngleVec *= sign;
  const double preyEndAngle = preyEndAngleVec.angle();

  Position bulletPos = attackingPos;
  const size_t maxIteration = (size_t)ceil(castRange / bulletSpeed.length());

  for (size_t iter = 0; iter <= maxIteration; iter++) {
    if ((bulletPos - attackingPos).length() > castRange) {
      bulletPos = bulletSpeed.normal() * castRange;
    }

    if ((bulletPos - preyPos).length() < bulletRadius + prey.getRadius()) {
      return false;
    }

    bulletPos += bulletSpeed;

    const auto speed = Algorithm::maxSpeed(prey, preyAngle, preyEndAngleVec);
    preyPos += preyEndAngleVec * speed.length();


    double angleDiff = Math::angleDiff(preyEndAngle, preyAngle);
    if (ABS(angleDiff) > preyTurnSpeed) {
      angleDiff = preyTurnSpeed * SIGN(angleDiff);
    }

    preyAngle += angleDiff;
  }

  return true;
}

bool Algorithm::canForwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius) {
  return canEscape(attackingPos, castRange, prey, bulletSpeed, bulletRadius, 1);
}

bool Algorithm::canBackwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius) {
  return canEscape(attackingPos, castRange, prey, bulletSpeed, bulletRadius, -1);
}