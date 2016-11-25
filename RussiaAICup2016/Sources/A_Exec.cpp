#include "A_Exec.h"
#include "C_Extensions.h"
#include "C_Math.h"

#include "E_Game.h"
#include "E_World.h"

using namespace AICup;

static std::vector<Position> stackForBugFix;

double calcMaxSpeed(const model::Wizard& self, const Vector& direction, const double speedLimit) {
  Vector speed = Vector(direction.x, -direction.y).normal().rotated(self.getAngle());

  double maxSpeed = (speed.x > 0) ? EX::maxSpeed(self) : EX::maxBackwardSpeed(self);
  const double sx = abs(speed.x);
  const double sy = abs(speed.y);
  maxSpeed = (maxSpeed * sx + EX::maxStrafeSpeed(self) * sy) / (sx + sy);

  if (speedLimit < 0) {
    return MIN(maxSpeed, direction.length());
  }
  return MIN(maxSpeed, MIN(speedLimit, direction.length()));
}


bool Algorithm::execMove(const model::Wizard& self, const TurnStyle style, const Vector& direction, const double speedLimit, model::Move& move) {
  Vector prespeed = Vector(direction.x, -direction.y).normal().rotated(self.getAngle());

  double maxSpeed = calcMaxSpeed(self, direction, speedLimit);

  const auto selfPos = EX::pos(self);
  const auto toPos = selfPos + direction.normal() * maxSpeed;


  Vector fixDirection = direction;
  for (const auto& obstacle : World::instance().obstacles(self, self.getRadius() + maxSpeed + 100/*������������ ������ � ����*/)) {
    const auto oPos = EX::pos(*obstacle);
    /// ���� �� ������������ �� ���� ��� � ������������, �� ������ �����������
    if (Math::distanceToSegment(oPos, selfPos, toPos) < self.getRadius() + obstacle->getRadius()) {
      const auto tangents = Math::tangetsForTwoCircle(selfPos, self.getRadius(), oPos, obstacle->getRadius());
      /// ��� ������ �������� ��� ������ ���� ����������
      if (tangents[0].dot(direction.normal()) < tangents[1].dot(direction.normal())) {
        fixDirection = tangents[1];
      } else {
        fixDirection = tangents[0];
      }

      break; /// � ����� ������� �� ��� ���������� ���������
    }
  }


  ///������ � ��� � �� ����� ��� ��� ����� ��������, �� ��� ���� ������� �� �����
  Vector speed = Vector(fixDirection.x, -fixDirection.y).normal().rotated(self.getAngle());
  speed.y *= -1;
  speed *= maxSpeed;


  move.setSpeed(speed.x);
  move.setStrafeSpeed(speed.y);

  switch (style) {
  case TurnStyle::TURN:
    move.setTurn(self.getAngleTo(self.getX() + direction.x, self.getY() + direction.y));
    break;
  case TurnStyle::NO_TURN:
    break;
  case TurnStyle::BACK_TURN:
    move.setTurn(self.getAngleTo(self.getX() - direction.x, self.getY() - direction.y));
    break;
  default:
    assert(false && "incorrect turn style");
  }

  return true;
}

bool Algorithm::execAttack(const model::Wizard& self, const model::ActionType action, const model::LivingUnit& unit, model::Move& move) {
  const double angleDeviation = self.getAngleTo(unit);

  /// � ����� ������ �������������� � �����, ��� ��������� ����� �����
  move.setTurn(angleDeviation);

  /// �� ��� ����, ����� ���������
  if (abs(angleDeviation) > Game::model().getStaffSector() * 0.5) {
    return false;
  }

  move.setCastAngle(angleDeviation);

  const double distance = self.getDistanceTo(unit);

  move.setMinCastDistance(distance - unit.getRadius());
  move.setMaxCastDistance(distance + unit.getRadius() * 2); // � �������
  move.setAction(action);
}