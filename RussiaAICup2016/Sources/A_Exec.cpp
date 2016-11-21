#include "A_Exec.h"
#include "C_Extensions.h"
#include "C_Math.h"

#include "E_Game.h"

using namespace AICup;

static std::vector<Position> stackForBugFix;

/// ���� �� �������� ��������� �� �������� ����� �������� ��� ���� � �� ����� �����/ ����� ����
void TwitchOperator(const model::Wizard& self, Vector& speed, model::Move& move) {
  const auto selfPos = EX::pos(self);

  stackForBugFix.push_back(EX::pos(self));
  while (stackForBugFix.size() > 5) {
    stackForBugFix.erase(stackForBugFix.begin());
  }

  for (const auto& data : stackForBugFix) {
    if ((data - selfPos).length() > EX::maxSpeed(self)) {
      return;
    }
  }

  speed.x = EX::maxSpeed(self) * (rand() % 1000) / 1000;
  speed.y = EX::maxStrafeSpeed(self) * (rand() % 1000) / 1000;
  move.setTurn(((rand() % 100) / 100) - 0.5);
  printf("Twitch\n");
}

bool Algorithm::execMove(const model::Wizard& self, const TurnStyle style, const Vector& direction, const double speedLimit, model::Move& move) {
  ///������ � ��� � �� ����� ��� ��� ����� ��������, �� ��� ���� ������� �� �����
  Vector speed = Vector(direction.x, -direction.y).normal().rotated(self.getAngle());
  speed.y *= -1;

  double maxSpeed = (speed.x > 0) ? EX::maxSpeed(self) : EX::maxBackwardSpeed(self);
  const double sx = abs(speed.x);
  const double sy = abs(speed.y);
  maxSpeed = (maxSpeed * sx + EX::maxStrafeSpeed(self) * sy)/(sx + sy);

  if (speedLimit < 0) {
    speed *= MIN(maxSpeed, direction.length());
  } else {
    speed *= MIN(maxSpeed, MIN(speedLimit, direction.length()));
  }

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

  //TwitchOperator(self, speed, move);

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