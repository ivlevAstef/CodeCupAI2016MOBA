#include "A_Exec.h"
#include "C_Extensions.h"
#include "C_Math.h"
#include "A_Move.h"

#include "E_Game.h"
#include "E_World.h"

using namespace AICup;

double calcMaxSpeed(const Wizard& self, const Vector& direction, const double speedLimit) {
  Vector speed = Algorithm::maxSpeed(self, self.getAngle(), direction);

  const double maxSpeed = speed.length();
  const double dirLength = direction.length();

  if (speedLimit < 0) {
    return MIN(maxSpeed, dirLength);
  }
  return MIN(maxSpeed, MIN(speedLimit, dirLength));
}


bool Algorithm::execMove(const Wizard& self, const TurnStyle style, const Vector& turnDirection, const Vector& direction, const double speedLimit, model::Move& move) {
  double maxSpeed = calcMaxSpeed(self, direction, speedLimit);

  const auto selfPos = EX::pos(self);
  const auto toPos = selfPos + direction.normal() * maxSpeed;

  ///������ � ��� � �� ����� ��� ��� ����� ��������, �� ��� ���� ������� �� �����
  Vector speed = Vector(direction.x, -direction.y).normal().rotated(self.getAngle());
  speed.y *= -1;
  speed *= maxSpeed;


  move.setSpeed(speed.x);
  move.setStrafeSpeed(speed.y);


  switch (style) {
  case TurnStyle::TURN:
    move.setTurn(self.getAngleTo(self.getX() + turnDirection.x, self.getY() + turnDirection.y));
    break;
  case TurnStyle::NO_TURN:
    break;
  case TurnStyle::BACK_TURN:
    move.setTurn(self.getAngleTo(self.getX() - turnDirection.x, self.getY() - turnDirection.y));
    break;
  case TurnStyle::SIDE_TURN: {
    auto perDirection = turnDirection.perpendicular();
    auto diff = Math::angleDiff(turnDirection.angle(), self.getAngle());
    /// ���� ��� ������ � ������ ��������, �� �������� ����������� ����� ���� � �����, ���� ��� ����������� �� ��������� � ������� �����
    if (perDirection.dot(Vector(1, 0).rotate(self.getAngle())) < 0) {
    /// ���� ���� ������ 90 �������� - ������ �� ����������� �� ���� ��������� ���������������
      perDirection *= -1;
    }

    move.setTurn(self.getAngleTo(self.getX() + perDirection.x, self.getY() + perDirection.y));
    break;
  }
  default:
    assert(false && "incorrect turn style");
  }

  return true;
}

bool Algorithm::execAroundMove(const Wizard& self, model::Move& move) {
  const auto selfPos = EX::pos(self);

  ///�������� ���������, �������������� � ���� ��� �������

  move.setSpeed((double(rand() % 1000) / 100.0) - 5.0);
  move.setStrafeSpeed((double(rand()%1000)/100.0) - 5.0);
  move.setTurn(1);

  if (0 == self.cooldown(model::ACTION_STAFF)) {
    move.setAction(model::ACTION_STAFF);
  } else if (0 == self.cooldown(model::ACTION_MAGIC_MISSILE)) {
    move.setAction(model::ACTION_MAGIC_MISSILE);
  }

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