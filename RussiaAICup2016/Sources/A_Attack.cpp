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

bool canEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius, const Vector preyEndVec, const double preyEndAngle) {
  auto preyPos = EX::pos(prey);
  auto preyAngle = prey.getAngle();
  auto preyTurnSpeed = EX::turnSpeed(prey);

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

    const auto speed = Algorithm::maxSpeed(prey, preyAngle, preyEndVec);
    preyPos += preyEndVec * speed.length();


    double angleDiff = Math::angleDiff(preyEndAngle, preyAngle);
    if (ABS(angleDiff) > preyTurnSpeed) {
      angleDiff = preyTurnSpeed * SIGN(angleDiff);
    }

    preyAngle += angleDiff;
  }

  return true;
}

bool Algorithm::canSideForwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius) {
  auto endAngleVec = bulletSpeed.perpendicular().normal();
  /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикул€ров
  if (endAngleVec.dot(Vector(1, 0).rotate(prey.getAngle())) < 0) {
    endAngleVec *= -1;
  }

  const double preyEndAngle = endAngleVec.angle();
  return canEscape(attackingPos, castRange, prey, bulletSpeed, bulletRadius, endAngleVec, preyEndAngle);
}
bool Algorithm::canSideBackwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius) {
  auto endAngleVec = bulletSpeed.perpendicular().normal();
  /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикул€ров
  if (endAngleVec.dot(Vector(1, 0).rotate(prey.getAngle())) < 0) {
    endAngleVec *= -1;
  }

  const double preyEndAngle = endAngleVec.angle();
  return canEscape(attackingPos, castRange, prey, bulletSpeed, bulletRadius, -endAngleVec, preyEndAngle);
}

bool Algorithm::canBackForwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius) {
  auto endAngleVec = bulletSpeed.normal();
  /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикул€ров
  if (endAngleVec.dot(Vector(1, 0).rotate(prey.getAngle())) < 0) {
    endAngleVec *= -1;
  }

  const double preyEndAngle = endAngleVec.angle();
  return canEscape(attackingPos, castRange, prey, bulletSpeed, bulletRadius, endAngleVec, preyEndAngle);
}
bool Algorithm::canBackBackwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius) {
  auto endAngleVec = bulletSpeed.normal();
  /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикул€ров
  if (endAngleVec.dot(Vector(1, 0).rotate(prey.getAngle())) < 0) {
    endAngleVec *= -1;
  }

  const double preyEndAngle = endAngleVec.angle();
  return canEscape(attackingPos, castRange, prey, bulletSpeed, bulletRadius, -endAngleVec, preyEndAngle);
}