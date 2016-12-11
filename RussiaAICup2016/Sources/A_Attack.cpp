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


bool canEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius, const Vector preyEndVec, const double preyEndAngle, const Obstacles& obstacles) {
  const auto preyBeginPos = EX::pos(prey);

  const size_t maxIteration = (size_t)ceil(castRange / bulletSpeed.length());

  /// находим максимальную дистанцию на которую можно сместиться при таком векторе, и не упереться
  auto preyEndPos = preyBeginPos + preyEndVec * (EX::maxSpeed(prey) * maxIteration);
  for (const auto& obstacle : obstacles) {
    /// пока не будет учитывать движимые объекты, ибо сложно
    if (EX::isMinion(*obstacle) || EX::isWizard(*obstacle)) {
      continue;
    }
    /// проверка на уперся в препятствие
    const auto obstaclePos = EX::pos(*obstacle);
    const auto intersectPoint = Math::point_distanceToSegment(EX::pos(*obstacle), preyBeginPos, preyEndPos);
    const auto distance = (obstaclePos - intersectPoint).length();
    const auto radius = obstacle->getRadius() + prey.getRadius();
    if (distance < radius) {
      double translate = sqrt(radius*radius - distance * distance);
      preyEndPos = intersectPoint - preyEndVec * translate;
    }
  }

  /// эмулируем движение
  const auto preyTurnSpeed = EX::turnSpeed(prey);
  auto preyPos = preyBeginPos;
  auto preyAngle = prey.getAngle();

  Position lastBulletPos = attackingPos;
  Position bulletPos = attackingPos;

  for (size_t iter = 0; iter <= maxIteration; iter++) {
    ///////////////////
    lastBulletPos = bulletPos;
    bulletPos += bulletSpeed;

    if ((bulletPos - attackingPos).length() > castRange) {
      bulletPos = attackingPos + bulletSpeed.normal() * castRange;
    }

    if (Math::distanceToSegment(preyPos, bulletPos, lastBulletPos) < bulletRadius + prey.getRadius()) {
      return false;
    }

    ////////////////////
    const auto speed = Algorithm::maxSpeed(prey, preyAngle, preyEndVec);
    preyPos += preyEndVec * speed.length();
    if ((preyPos - preyBeginPos).length2() > (preyEndPos - preyBeginPos).length2()) {
      preyPos = preyEndPos;
    }

    ////////////////////
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
  /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикуляров
  if (endAngleVec.dot(Vector(1, 0).rotate(prey.getAngle())) < 0) {
    endAngleVec *= -1;
  }

  const double preyEndAngle = endAngleVec.angle();
  const auto obstacles = World::instance().obstacles(prey, prey.getRadius() + bulletRadius);
  return canEscape(attackingPos, castRange, prey, bulletSpeed, bulletRadius, endAngleVec, preyEndAngle, obstacles);
}
bool Algorithm::canSideBackwardEscape(const Position attackingPos, const double castRange, const model::Wizard& prey, const Vector bulletSpeed, const double bulletRadius) {
  auto endAngleVec = bulletSpeed.perpendicular().normal();
  /// если угол больше 90 градусов - выбран не оптимальный из двух возможных перпендикуляров
  if (endAngleVec.dot(Vector(1, 0).rotate(prey.getAngle())) < 0) {
    endAngleVec *= -1;
  }

  const double preyEndAngle = endAngleVec.angle();
  const auto obstacles = World::instance().obstacles(prey, prey.getRadius() + bulletRadius);
  return canEscape(attackingPos, castRange, prey, bulletSpeed, bulletRadius, -endAngleVec, preyEndAngle, obstacles);
}

bool Algorithm::canDodge(const model::Wizard& prey, const Vector desiredDir, const Bullet bullet) {
  int turnSign = 0;
  return dodge(prey, desiredDir, bullet, turnSign).length() > 0.1;
}

Vector Algorithm::dodge(const model::Wizard& prey, const Vector desiredDir, const Bullet bullet, int& turnSign) {
  const double centerAngle = desiredDir.angle();

  const auto obstacles = World::instance().obstacles(prey, prey.getRadius() + bullet.radius + 100);

  /// проверяем возможность уклониться при всех возможных конечных углах, начиная с текущего
  /// проверяем каждые 6 градусов
  for (double angleDt = 0; angleDt < AICUP_PI; angleDt += AICUP_PI / 30.0) {
    const auto leftAngle = centerAngle + angleDt;
    const auto rightAngle = centerAngle - angleDt;
    const auto endVectorLeft = Vector(1, 0).rotate(leftAngle);
    const auto endVectorRight = Vector(1, 0).rotate(rightAngle);

    /// если идти вперед
    if (canEscape(bullet.startPoint, bullet.range, prey, bullet.speed, bullet.radius, endVectorLeft, leftAngle, obstacles)) {
      turnSign = 1;
      return endVectorLeft;
    }

    if (canEscape(bullet.startPoint, bullet.range, prey, bullet.speed, bullet.radius, endVectorRight, rightAngle, obstacles)) {
      turnSign = 1;
      return endVectorRight;
    }

    /// если идти назад
    if (canEscape(bullet.startPoint, bullet.range, prey, bullet.speed, bullet.radius, -endVectorLeft, leftAngle, obstacles)) {
      turnSign = -1;
      return -endVectorLeft;
    }
    if (canEscape(bullet.startPoint, bullet.range, prey, bullet.speed, bullet.radius, -endVectorRight, rightAngle, obstacles)) {
      turnSign = -1;
      return -endVectorRight;
    }
  }

  return Vector();
}

bool Algorithm::canAttackMMOrMelee(const model::Wizard& self, const model::LivingUnit& unit) {
  const double timeToAttack = timeToTurnForAttack(unit, self);
  const int cooldownStaff = EX::cooldownSkill(self, model::ACTION_STAFF);
  const int cooldownMM = EX::cooldownSkill(self, model::ACTION_MAGIC_MISSILE);

  bool melee = isMelee(self, unit);

  /// если далеко, и еще долго до дальнего боя.
  if (!melee && cooldownMM > timeToAttack + 1) {
    return false;
  }
  // если близко, но бить пока нечем
  if (melee && MIN(cooldownStaff, cooldownMM) > timeToAttack + 1) {
    return false;
  }

  return true;
}

bool Algorithm::isImmortal(const model::Building& building) {
  const auto lane = World::instance().positionToLine(building.getX(), building.getY());

  /// если всего одна вышка, то точно не бессмертна
  if (World::instance().towerCount(lane, building.getFaction()) <= 1) {
    return false;
  }

  /// иначе посмотреть какая она ближняя или дальняя
  const model::Building* base = nullptr;
  const model::Building* other = nullptr;
  for (const auto& build : World::instance().buildings()) {
    if (build.getFaction() != building.getFaction() || build.getId() == building.getId()) {
      continue;
    }

    if (build.getType() == model::BUILDING_FACTION_BASE) {
      base = &build;
    } else if (World::instance().positionToLine(build.getX(), build.getY()) == lane) {
      other = &build;
    }
  }

  if (nullptr == base || nullptr == other) {
    return false;
  }

  /// если здание ближе к базе, значит бесмертное
  return base->getDistanceTo(building) < base->getDistanceTo(*other);
}