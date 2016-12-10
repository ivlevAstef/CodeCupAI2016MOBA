#include "A_Exec.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "A_Move.h"

#include "E_Game.h"
#include "E_World.h"

using namespace AICup;

double angleDiff(const Wizard& self, const Vector& turnDirection) {
  double angleDiff = Math::angleDiff(turnDirection.angle(), self.getAngle());
  const auto preyTurnSpeed = EX::turnSpeed(self);
  if (ABS(angleDiff) > preyTurnSpeed) {
    angleDiff = preyTurnSpeed * SIGN(angleDiff);
  }
  return angleDiff;
}

double calcMaxSpeed(const Wizard& self, const Vector& turnDirection, const Vector& direction) {
  Vector speed = Algorithm::maxSpeed(self, self.getAngle(), direction);

  const double maxSpeed = speed.length();
  const double dirLength = direction.length();

  return MIN(maxSpeed, dirLength);
}


bool Algorithm::execMove(const Wizard& self, const Vector& turnDirection, const Vector& direction, model::Move& move) {
  double maxSpeed = calcMaxSpeed(self, turnDirection, direction);

  const auto selfPos = EX::pos(self);
  const auto toPos = selfPos + direction.normal() * maxSpeed;

  ///������ � ��� � �� ����� ��� ��� ����� ��������, �� ��� ���� ������� �� �����
  Vector speed = Vector(direction.x, direction.y).rotated(-self.getAngle()).normal();
  speed *= maxSpeed;

  const double turn = angleDiff(self, turnDirection);


  move.setSpeed(speed.x);
  move.setStrafeSpeed(speed.y);
  move.setTurn(turn);

  return true;
}

bool Algorithm::execAttack(const Wizard& self, const model::ActionType action, const model::LivingUnit& unit, model::Move& move) {
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

  return true;
}


bool Algorithm::execCast(const Wizard& self, const model::ActionType action, const model::LivingUnit& unit, model::Move& move) {
  const double angleDeviation = self.getAngleTo(unit);

  /// � ����� ������ �������������� � �����, ��� ��������� ����� �����
  move.setTurn(angleDeviation);

  /// �� ��� ����, ����� ���������
  if (unit.getId() != self.getId() && abs(angleDeviation) > Game::model().getStaffSector() * 0.5) {
    return false;
  }

  move.setStatusTargetId(unit.getId());
  move.setAction(action);

  return true;
}