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


bool canEscape(const Bullet& bullet, const model::Wizard& prey, const Position& preyBeginPos, const Vector preyEndVec, const double preyEndAngle, const Obstacles& obstacles) {
  const double subRange = (bullet.pos - bullet.startPoint).length();
  const size_t maxIteration = (size_t)ceil((bullet.range - subRange) / bullet.speed.length());

  /// ������� ������������ ��������, ����� ��� ��������������� �������� ������� ����� ����� ����� ����� �� �������
  const auto currentMaxSpeed = Algorithm::maxSpeed(prey, prey.getAngle(), preyEndVec).length();
  const auto maxSpeed = EX::maxSpeed(prey);
  /// ������� ������������ ��������� �� ������� ����� ���������� ��� ����� �������, � �� ���������
  auto preyEndPos = preyBeginPos + preyEndVec * (EX::maxSpeed(prey) * double(maxIteration + 1));
  for (const auto& obstacle : obstacles) {
    /// �������� �� ������ � �����������
    const auto obstaclePos = EX::pos(*obstacle);
    const auto intersectPoint = Math::point_distanceToSegment(obstaclePos, preyBeginPos, preyEndPos);
    const auto distance = (obstaclePos - intersectPoint).length();
    auto radius = obstacle->getRadius() + prey.getRadius();

    /// ��� ���������� ��������, ������� ������ �� �������� �������� ������
    if (distance > radius) {
      continue;
    }

    // �������� ������ ��� ������� ������, �� � ���� ����������� ������ �� ����������, � ����������� �� ��� ��������
    int ticks = (preyBeginPos - intersectPoint).length() / maxSpeed;
    if (EX::isWizard(*obstacle)) {
      /// ���� �� ����� ���������� ��������, ���� ����� �����.
      radius -= double(ticks) * EX::maxSpeed(*obstacle) * 0.1;
    } else if (EX::isMinion(*obstacle)) {
      /// ������� ���� ������ ������
      double speedMult = abs(preyEndVec.perpendicular().dot(Vector(1, 0).rotated(obstacle->getAngle())));
      radius -= double(ticks) * EX::maxSpeed(*obstacle) * speedMult * 0.5;
    }


    if (distance < radius) {
      double translate = sqrt(radius*radius - distance * distance);
      const auto& prePos = intersectPoint - preyEndVec * translate;
      if ((prePos - preyBeginPos).length() < (preyEndPos - preyBeginPos).length()) {
        preyEndPos = prePos;
      }
    }
  }

  /// ��������� ��������
  const auto preyTurnSpeed = EX::turnSpeed(prey);
  auto preyPos = preyBeginPos;
  auto preyAngle = prey.getAngle();

  Position lastBulletPos = bullet.pos;
  Position bulletPos = bullet.pos;

  for (size_t iter = 0; iter <= maxIteration; iter++) {
    ///////////////////
    lastBulletPos = bulletPos;
    bulletPos += bullet.speed;

    if ((bulletPos - bullet.startPoint).length() > bullet.range) {
      bulletPos = bullet.startPoint + bullet.speed.normal() * bullet.range;
    }

    if (bulletPos == lastBulletPos) {
      break;
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

    if (Math::distanceToSegment(preyPos, bulletPos, lastBulletPos) < bullet.radius + prey.getRadius()) {
      return false;
    }
  }

  return true;
}

Vector _dodge(const model::Wizard& prey, const Position& preyBeginPos, const Vector desiredDir, const Bullet bullet, int& turnSign) {
  const double centerAngle = desiredDir.angle();

  const auto obstacles = World::instance().obstacles(prey, prey.getRadius() + bullet.radius + 200);

  /// ��������� ����������� ���������� ��� ���� ��������� �������� �����, ������� � ��������
  const double dtStep = EX::turnSpeed(prey) * 0.5;
  for (double angleDt = 0; angleDt < 3.1416 + dtStep; angleDt += dtStep) {
    const auto leftAngle = centerAngle + angleDt;
    const auto rightAngle = centerAngle - angleDt;
    const auto endVectorLeft = Vector(1, 0).rotate(leftAngle);
    const auto endVectorRight = Vector(1, 0).rotate(rightAngle);

    /// ���� ���� ������
    if (canEscape(bullet, prey, preyBeginPos, endVectorLeft, leftAngle, obstacles)) {
      turnSign = 1;
      return endVectorLeft;
    }

    if (canEscape(bullet, prey, preyBeginPos, endVectorRight, rightAngle, obstacles)) {
      turnSign = 1;
      return endVectorRight;
    }

    /// ���� ���� �����
    if (canEscape(bullet, prey, preyBeginPos, -endVectorLeft, leftAngle, obstacles)) {
      turnSign = -1;
      return -endVectorLeft;
    }
    if (canEscape(bullet, prey, preyBeginPos, -endVectorRight, rightAngle, obstacles)) {
      turnSign = -1;
      return -endVectorRight;
    }
  }

  return Vector();
}

bool Algorithm::canDodge(const model::Wizard& prey, const Vector desiredDir, const Bullet bullet) {
  int turnSign = 0;
  const auto preyBeginPos = EX::pos(prey);
  return _dodge(prey, preyBeginPos, desiredDir, bullet, turnSign).length() > 0.1;
}

bool Algorithm::canDodge(const model::Wizard& prey, const Position& preyBeginPos, const Vector desiredDir, const Bullet bullet) {
  int turnSign = 0;
  return _dodge(prey, preyBeginPos, desiredDir, bullet, turnSign).length() > 0.1;
}

Vector Algorithm::dodge(const model::Wizard& prey, const Vector desiredDir, const Bullet bullet, int& turnSign) {
  const auto preyBeginPos = EX::pos(prey);
  return _dodge(prey, preyBeginPos, desiredDir, bullet, turnSign);
}


bool Algorithm::canAttackMMOrMelee(const model::Wizard& self, const model::LivingUnit& unit) {
  const double timeToAttack = timeToTurnForAttack(unit, self);
  const int cooldownStaff = EX::cooldownSkill(self, model::ACTION_STAFF);
  const int cooldownMM = EX::cooldownSkill(self, model::ACTION_MAGIC_MISSILE);

  bool melee = isMelee(self, unit);

  /// ���� ������, � ��� ����� �� �������� ���.
  if (!melee && cooldownMM > timeToAttack + 1) {
    return false;
  }
  // ���� ������, �� ���� ���� �����
  if (melee && MIN(cooldownStaff, cooldownMM) > timeToAttack + 1) {
    return false;
  }

  return true;
}

bool Algorithm::isImmortal(const model::Building& building) {
  const auto lane = World::instance().positionToLine(building.getX(), building.getY());

  /// ���� ����� ���� �����, �� ����� �� ����������
  if (World::instance().towerCount(lane, building.getFaction()) <= 1) {
    return false;
  }

  /// ����� ���������� ����� ��� ������� ��� �������
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

  /// ���� ������ ����� � ����, ������ ����������
  return base->getDistanceTo(building) < base->getDistanceTo(*other);
}