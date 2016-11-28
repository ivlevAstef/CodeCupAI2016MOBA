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

  const double needTurnAngle = MAX(0, angleDeviation - Game::instance().model().getStaffSector()* 0.5);
  /// если времени до атаки больше чем времени до разворота, то можно пока не атаковать

  return needTurnAngle / EX::turnSpeed(attacking);
}

bool Algorithm::canEscape(const model::Wizard& attacking, const model::Wizard& prey, const double bulletSpeed, const double bulletRadius) {
  const auto attackingPos = EX::pos(attacking);
  const auto preyForwardSpeed = EX::maxSpeed(prey);
  const auto preyStrafeSpeed = EX::maxStrafeSpeed(prey);

  auto preyPos = EX::pos(prey);
  auto preyAngle = prey.getAngle();
  auto preyTurnSpeed = EX::turnSpeed(prey);

  const Vector bulletSpeedVec = (preyPos - attackingPos).normal() * bulletSpeed;

  auto preyEndAngleVec = bulletSpeedVec.perpendicular().normal();
  /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикуляров
  if (preyEndAngleVec.dot(Vector(1, 0).rotate(preyAngle)) < 0) {
    preyEndAngleVec *= -1;
  }
  const double preyEndAngle = preyEndAngleVec.angle();

  Position bulletPos = attackingPos;
  const size_t maxIteration = (size_t)ceil(attacking.getCastRange() / bulletSpeed);

  for (size_t iter = 0; iter <= maxIteration; iter++) {
    if ((bulletPos - attackingPos).length() > attacking.getCastRange()) {
      bulletPos = bulletSpeedVec.normal() * attacking.getCastRange();
    }

    if ((bulletPos - preyPos).length() < bulletRadius + prey.getRadius()) {
      return false;
    }

    bulletPos += bulletSpeedVec;

    const auto speed = Algorithm::maxSpeed(prey, preyEndAngleVec);
    preyPos += preyEndAngleVec * speed.length();


    double angleDiff = Math::angleDiff(preyEndAngle, preyAngle);
    if (ABS(angleDiff) > preyTurnSpeed) {
      angleDiff = preyTurnSpeed * SIGN(angleDiff);
    }

    preyAngle += angleDiff;
  }

  return true;
}